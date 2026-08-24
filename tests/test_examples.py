"""Run the example programs and check they match CPython."""

import glob

import pytest

from splice.pipeline import pipeline
from splice.cpp_build import compile_and_run
from tests.test_utils import print_output_diff, run_python_and_capture

examples_path = "examples"


class TestExamples:
    paths = glob.glob(f"{examples_path}/*.py")

    @pytest.mark.parametrize("filename", paths, ids=lambda p: p.split("/")[-1])
    def test_example(self, filename: str, tmp_path):
        """An example must produce identical output in Python and C++."""
        program = open(filename).read()
        cpp_program = pipeline(filename, program)
        cpp_output = compile_and_run(
            cpp_program,
            str(tmp_path / "main.cpp"),
            str(tmp_path / "main"),
            filename,
        )
        python_output = run_python_and_capture(filename)
        print_output_diff(python_output.stdout, cpp_output.stdout)
        assert cpp_output.stdout == python_output.stdout
