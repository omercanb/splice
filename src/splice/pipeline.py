import sys
from dataclasses import dataclass

from mypy import build
from mypy.main import define_options
from mypy.modulefinder import BuildSource
from mypy.nodes import (
    Expression,
    MypyFile,
    TypeInfo,
)
from mypy.options import Options
from mypy.types import Instance, Type

from splice.ast_utils import TypeTable
from splice.analysis.call_graph import compute_call_graph
from splice.analysis.mutation import MutationTable, compute_mutating_parameters
from splice.analysis.statement_effects import compute_function_effects
from splice.frontend.mypy_fixes import get_resolved_types
from splice.frontend.validate import validate, UnsupportedProgram, render
from splice.frontend.validate_semantics import validate_semantics
from splice.codegen.statement_codegen import StatementCodegen
from splice.transform.comprehension_transformer import (
    apply_comprehension_transforms,
)
from splice.transform.docstring_transformer import DocstringRemover
from splice.transform.index_transformer import IndexTransformer

_STRICT_ASSIGNMENTS = define_options()[2]

function_fallback: Instance


@dataclass
class AnalysisResult:
    tree: MypyFile
    types: TypeTable
    mutations: MutationTable
    source: str
    path: str | None


def mypy_options():
    opts = Options()
    opts.export_types = True  # required to populate result.types
    opts.preserve_asts = True  # keep the trees alive after checking
    # Caching typeshed takes a build from ~0.45s to ~0.015s
    # Make sure to always give the source when using BuildSource
    # because with this on it will otherwise just skip the file
    opts.incremental = True
    opts.cache_dir = ".mypy_cache"
    for option, value in _STRICT_ASSIGNMENTS:
        setattr(opts, option, value)
    opts.strict_optional = True
    return opts


def parse(path: str | None, source: str) -> MypyFile:
    # Path is an optional parameter because it's only used for error messages
    result = build.build(
        sources=[BuildSource(path, "main", source)], options=mypy_options()
    )
    if result.errors:
        print("\n".join(result.errors))
        sys.exit(1)
    tree = result.files["main"]
    # See analyse()'s comment: build() leaks its cache connections otherwise.
    result.manager.metastore.close()
    return tree


def _set_global_function_fallback(result: build.BuildResult):
    """Used when creating custom function definitions directly using the ast"""
    global function_fallback
    sym = result.files["builtins"].names["function"]
    assert sym.node is not None
    function_type_info = sym.node
    assert isinstance(function_type_info, TypeInfo)
    function_fallback = Instance(function_type_info, [])


def analyse(
    path: str | None, source: str, check_semantics: bool = True
) -> AnalysisResult:
    """
    The mypy analysis pass
    Does type inference, defines classes, resolves names
    Does some additional work on top of just running mypy

    check_semantics=False skips the value-semantics checks (copy(),
    exclusivity, ...) while still returning the fully transformed tree -
    for tools and tests that only need the tree, not a verdict on it.
    """
    # Path is an optional parameter because it's only used for error messages
    result = build.build(
        sources=[BuildSource(path, "main", source)], options=mypy_options()
    )
    if result.errors:
        print("\n".join(result.errors))
        sys.exit(1)
    tree = result.files["main"]

    _set_global_function_fallback(result)

    types = get_resolved_types(result.files["main"], result.types)
    # We need to close() to free a SQLite connection, otherwise we get too many connections when running tests
    result.manager.metastore.close()
    diagnostics = validate(tree, types)
    if diagnostics:
        print(render(diagnostics, source, path))
        raise UnsupportedProgram(diagnostics)

    _apply_transforms(tree, types)

    function_effects = compute_function_effects(tree, types)
    call_graph = compute_call_graph(tree, types)
    mutations = compute_mutating_parameters(function_effects, call_graph)

    if check_semantics:
        semantics_diagnostics = validate_semantics(tree, mutations, types, source)
        if semantics_diagnostics:
            print(render(semantics_diagnostics, source, path))
            raise UnsupportedProgram(semantics_diagnostics)

    return AnalysisResult(result.files["main"], types, mutations, source, path)


def _apply_transforms(tree: MypyFile, types: dict[Expression, Type]):
    DocstringRemover().visit(tree)
    apply_comprehension_transforms(tree, types)
    IndexTransformer(types).visit(tree)


def generate(result: AnalysisResult) -> str:
    """Translate the already-validated, already-transformed tree."""
    return StatementCodegen(result.tree, result.types, result.mutations).generate()


def pipeline(path: str, source: str) -> str:
    result = analyse(path, source)
    return generate(result)


