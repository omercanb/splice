import pytest

from splice import namer


@pytest.fixture(autouse=True)
def _reset_namer():
    """Temp names are a global counter, so two tests that both trigger a
    transform get names depending on run order unless reset in between.
    """
    namer.reset()


def pytest_addoption(parser):
    parser.addoption(
        "--separate-compile",
        action="store_true",
        help="Compile each test program on its own instead of batching them all "
        "into one binary. Slower, but a compile error names the program it came "
        "from rather than a line in the combined source.",
    )
