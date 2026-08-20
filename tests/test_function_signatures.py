"""Test that function signatures are correctly translated to C++."""

import pytest
from mypy.nodes import FuncDef, TypeInfo

from splice.pipeline import analyse
from splice.codegen.expression_codegen import ExpressionCodegen
from splice.codegen.translation_utils import translate_func_signature

# Test Python functions with various signatures
test_code = """
def simple(x: int) -> int:
    return x + 1

def with_defaults(a: int, b: str = "hello", c: float = 3.14) -> str:
    return b

def multiple_params(x: int, y: float, z: bool) -> str:
    return "result"

def no_return() -> None:
    pass

def returns_list(items: list[int]) -> list[str]:
    return ["a", "b"]

def returns_dict(d: dict[str, int]) -> dict[int, str]:
    return {1: "one"}

def no_params() -> int:
    return 42

def with_optional(x: int | None) -> str:
    return "ok"

def with_object(x: B) -> B:
    return B()

class B:
    pass

class A:
    def method(self, other: int) -> int:
        return 1
"""

function_signatures = {
    "simple": "int64_t simple(const int64_t &x)",
    "with_defaults": 'str with_defaults(const int64_t &a, const str &b = str("hello"), const double &c = 3.14)',
    "multiple_params": "str multiple_params(const int64_t &x, const double &y, const bool &z)",
    "no_return": "void no_return()",
    "returns_list": "list<str> returns_list(const list<int64_t> &items)",
    "returns_dict": "dict<int64_t, str> returns_dict(const dict<str, int64_t> &d)",
    "no_params": "int64_t no_params()",
    "with_optional": "str with_optional(const std::optional<int64_t> &x)",
    "with_object": "B with_object(const B &x)",
}

class_name = "A"
method_signatures = {"method": "int64_t method(const int64_t &other)"}


class TestFunctionSignatures:
    """Test C++ function signature generation."""

    @classmethod
    def setup_class(cls):
        result = analyse(None, test_code, check_semantics=False)
        cls.tree = result.tree
        cls.types = result.types
        cls.mutations = result.mutations
        cls.expr_translator = ExpressionCodegen(result.types)

    def generate_function_signature(self, func_name: str) -> str:
        """Generate C++ signature for a function."""
        sym = self.tree.names.get(func_name)
        assert sym and isinstance(sym.node, FuncDef)
        signature = translate_func_signature(
            sym.node, self.expr_translator, mutations=self.mutations
        )
        return signature

    def generate_method_signature(self, method_name: str, class_name: str) -> str:
        class_info = self.tree.names.get(class_name)
        assert class_info and isinstance(class_info.node, TypeInfo)
        method = class_info.node.names[method_name].node
        assert isinstance(method, FuncDef)
        signature = translate_func_signature(
            method, self.expr_translator, mutations=self.mutations
        )
        return signature

    @pytest.mark.parametrize("func_name,expected_sig", function_signatures.items())
    def test_func_signature(self, func_name, expected_sig):
        """Test function signature."""
        sig = self.generate_function_signature(func_name)
        assert (
            sig == expected_sig
        ), f"Mismatch for {func_name}:\nGot:      {sig}\nExpected: {expected_sig}"

    @pytest.mark.parametrize("func_name,expected_sig", method_signatures.items())
    def test_method_signature(self, func_name, expected_sig):
        """Test method signature."""
        sig = self.generate_method_signature(func_name, class_name)
        assert (
            sig == expected_sig
        ), f"Mismatch for {func_name}:\nGot:      {sig}\nExpected: {expected_sig}"
