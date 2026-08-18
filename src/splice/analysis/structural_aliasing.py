"""
Analyze wether two expressions can alias the same place in memory
Eg: 'a.field' and 'a' overlap
    'a.list' and 'a.list[0]' overlap
    'l[i]' and 'l[j]' may overlap
    'a.field1' and 'a.field2' don't overlap
    'l[0]' and 'l[1]' don't overlap
Note that this is only valid for when variables are the root.
An access path is how the final value is accessed.
Eg: a.list[i].field has access path (Root: a, (Member: list, Method: getitem(i), Member: field))
"""

# Core idea first split an expression into an access path which is a root var + a list of projections
# Projections are a Field(name), Method(name) where the Method is an aliasing method, __getitem__ or back()

from dataclasses import dataclass
from typing import Optional

from mypy.nodes import CallExpr, Expression, MemberExpr, NameExpr, Var

from splice.analysis.mutation import _ALIASING_METHODS
from splice.ast_utils import get_int_literal


@dataclass
class AccessPath:
    root: Var
    projections: list[MemberExpr | CallExpr]


def get_access_path(expr: Expression) -> Optional[AccessPath]:
    """Get an access path if one exists for the expression"""
    if isinstance(expr, NameExpr):
        if isinstance(expr.node, Var):
            return AccessPath(expr.node, [])
        return None
    elif isinstance(expr, MemberExpr):
        base = get_access_path(expr.expr)
        if base is None:
            return None
        base.projections.append(expr)
        return base
    elif isinstance(expr, CallExpr):
        if (
            isinstance(expr.callee, MemberExpr)
            and expr.callee.name in _ALIASING_METHODS
        ):
            # We only care about a call if its back() or getitem() which alias the element
            base = get_access_path(expr.callee.expr)
            if base is None:
                return None
            base.projections.append(expr)
            return base
        return None
    else:
        return None


# A function to compare two paths and check if they could overlap
def is_access_path_structural_alias(
    path1: AccessPath, path2: AccessPath
) -> Optional[AccessPath]:
    """Checks wether two paths are structural alias and returns the common path if it exists"""
    if path1.root != path2.root:
        return None
    common_path = AccessPath(path1.root, [])

    for projection1, projection2 in zip(path1.projections, path2.projections):
        if projection1.__class__ != projection2.__class__:
            # Accesses are of different types (one is a field and one is an index)
            return None

        elif isinstance(projection1, MemberExpr) and isinstance(
            projection2, MemberExpr
        ):
            if projection1.name != projection2.name:
                # Different fields means no aliasing
                return None
            common_path.projections.append(projection1)
        elif isinstance(projection1, CallExpr) and isinstance(projection2, CallExpr):
            assert isinstance(projection1.callee, MemberExpr)
            assert isinstance(projection2.callee, MemberExpr)

            # The functions are either back() or __getitem__(index)
            f1_name = projection1.callee.name
            f2_name = projection2.callee.name

            back = "back"
            getitem = "__getitem__"

            if f1_name == f2_name == back:  # Both directly alias to the last element
                common_path.projections.append(projection1)
            elif (f1_name == back and f2_name == getitem) or (
                f1_name == getitem and f2_name == back
            ):  # Back can alias getitem eg: l = [0, 1], l[-1] -> l.back(), l[1] -> l.getitem(1) both the same element
                common_path.projections.append(projection1)
            elif f1_name == getitem and f2_name == getitem:
                # If the indices are different literals no aliasing, otherwise aliasing
                index1 = projection1.args[0]
                index2 = projection2.args[0]
                index_literal1 = get_int_literal(index1)
                index_literal2 = get_int_literal(index2)
                if index_literal1 is None or index_literal2 is None:
                    # Can't prove indexes are distinct
                    common_path.projections.append(projection1)
                elif index_literal1 == index_literal2:
                    common_path.projections.append(projection1)
                else:
                    # Index literals are confirmed distinct
                    return None
            else:
                assert False, f"unhandled aliasing method combination: {f1_name}/{f2_name}"
        else:
            assert False
    return common_path
