"""ArgumentTransformer: a call to a user-defined function (or constructor) is
rewritten to be fully positional, in the callee's declared parameter order,
splicing in a default for anything the caller omitted.
"""

from pathlib import Path

from splice.convert_to_python import PythonPrinter
from splice.pipeline import analyse

test_file = Path(__file__).parent / "program.py"


def test_argument_transform_snapshot(snapshot):
    program = analyse(str(test_file), open(test_file).read(), check_semantics=False)
    back_to_python = PythonPrinter().visit(program.tree)
    assert snapshot == back_to_python
