"""Which functions/methods are marked @hotpath, for codegen's FLATTEN."""

from mypy.nodes import ClassDef, Decorator, FuncDef, MypyFile

_HOTPATH_FULLNAME = "splice.stdlib.hotpath"


def _is_hotpath(decorator: Decorator) -> bool:
    return any(
        getattr(expr, "fullname", None) == _HOTPATH_FULLNAME
        for expr in decorator.decorators
    )


def collect_hotpath_funcs(tree: MypyFile) -> set[FuncDef]:
    """Every FuncDef (free function or method) decorated with @hotpath."""
    found: set[FuncDef] = set()
    for d in tree.defs:
        if isinstance(d, Decorator) and _is_hotpath(d):
            found.add(d.func)
        elif isinstance(d, ClassDef):
            for stmt in d.defs.body:
                if isinstance(stmt, Decorator) and _is_hotpath(stmt):
                    found.add(stmt.func)
    return found
