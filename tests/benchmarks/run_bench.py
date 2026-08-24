from __future__ import annotations

import argparse
import glob
import os
import re
import statistics
import subprocess
import sys
from enum import Enum, auto
from importlib import import_module
from pathlib import Path
from typing import NamedTuple, Optional

from tabulate import tabulate

from splice import namer
from splice.cpp_build import compile, compile_cpp
from splice.pipeline import pipeline
from tests.benchmarks.benchmarking import BenchmarkInfo, benchmarks

raw_output = False

# Minimum total time (seconds) to run a benchmark
MIN_TIME = 2.0
# Minimum number of iterations to run a benchmark
MIN_ITER = 10

# A cache of the translated executables that gets populated when first translated
translated_executable_paths: dict[str, str] = {}
# A cache of the compiled handwritten files
handwritten_executable_paths: dict[str, str] = {}

benchmarks_root_dir = Path(__file__).parent
benchmarks_dir = benchmarks_root_dir / "benchmarks"
microbenchmarks_dir = benchmarks_root_dir / "microbenchmarks"


class BenchmarkMode(Enum):
    interpreted = auto()  # Running the benchamrk with python
    translated = auto()  # Running the benchmark after translating
    handwritten = auto()  # Running the custom written C++ version of the benchmark


def run_once(benchmark: BenchmarkInfo, mode: BenchmarkMode) -> float:
    if mode == BenchmarkMode.interpreted:
        return run_interpreted(benchmark)
    elif mode == BenchmarkMode.translated:
        return run_translated(benchmark)
    elif mode == BenchmarkMode.handwritten:
        return run_handwritten(benchmark)
    assert False


def run_interpreted(benchmark: BenchmarkInfo) -> float:
    module = benchmark.module
    program = (
        'import %s; import tests.benchmarks.benchmarking as bm; print("\\nelapsed:", bm.run_once("%s"))'
        % (
            module,
            benchmark.name,
        )
    )
    cmd = [sys.executable, "-c", program]
    result = subprocess.run(cmd, check=True, stdout=subprocess.PIPE)
    return parse_elapsed_time(result.stdout)


def run_translated(benchmark: BenchmarkInfo) -> float:
    if benchmark.name not in translated_executable_paths:
        translated_executable_paths[benchmark.name] = translate_and_compile_benchmark(
            benchmark
        )
    binary = translated_executable_paths[benchmark.name]
    cmd = [f"./{binary}"]
    result = subprocess.run(cmd, check=True, stdout=subprocess.PIPE)
    return parse_elapsed_time(result.stdout)


def run_handwritten(benchmark: BenchmarkInfo) -> float:
    if benchmark.name not in handwritten_executable_paths:
        handwritten_executable_paths[benchmark.name] = compile_handwritten_benchmark(
            benchmark
        )
    binary = handwritten_executable_paths[benchmark.name]
    cmd = [f"./{binary}"]
    result = subprocess.run(cmd, check=True, stdout=subprocess.PIPE)
    return parse_elapsed_time(result.stdout)


def parse_elapsed_time(output: bytes) -> float:
    m = re.search(rb"\belapsed: ([-+0-9.e]+)\b", output)
    assert m is not None, "could not find elapsed time in output:\n%r" % output
    return float(m.group(1))


def smoothen(a: list[float]) -> list[float]:
    # Remove (at most) one third of the slowest runs (these are likely outliers).
    return sorted(a)[: 2 * (len(a) + 1) // 3]


def run_benchmark(
    benchmark: BenchmarkInfo,
    mode: BenchmarkMode,
    min_iter: Optional[int] = None,
    min_time: Optional[float] = None,  # Min time in seconds
) -> list[float]:
    namer.reset()

    if not min_iter:
        # Use default minimum iterations
        if benchmark.min_iterations is not None:
            min_iter = benchmark.min_iterations
        else:
            min_iter = MIN_ITER
    if not min_time:
        min_time = 2

    if not raw_output:
        print("running %s : %s" % (benchmark.name, mode.name))

    # Warm up
    run_once(benchmark, mode)

    times = []
    n = 0
    while True:
        t = run_once(benchmark, mode)
        times.append(t)
        if not raw_output:
            sys.stdout.write(".")
            sys.stdout.flush()
        n += 1
        long_enough = sum(times) >= MIN_TIME
        if long_enough and n >= min_iter:
            break

    if not raw_output:
        print()
    return times


def get_statistics(times: list[float]):
    return (statistics.mean(times), statistics.stdev(times))


def get_python_filepath(benchmark: BenchmarkInfo) -> str:
    fname = benchmark.module.replace(".", "/") + ".py"
    return fname


def get_translated_filepath(benchmark: BenchmarkInfo) -> str:
    fname = benchmark.module.replace(".", "/") + ".cpp"
    return fname


def get_handwritten_filepath(benchmark: BenchmarkInfo) -> str:
    module_path = benchmark.module.replace(".", "/")
    separator = module_path.rfind("/")
    path = f"{module_path[:separator]}/{module_path[separator + 1:]}_hw.cpp"
    return path


def translate_and_compile_benchmark(benchmark: BenchmarkInfo) -> str:
    if not raw_output:
        print(f"compiling {benchmark.name} {benchmark.module}...")
    cpp = translate_module_for_benchmarking(benchmark.module, benchmark.name)
    cpp_path = get_translated_filepath(benchmark)
    exe = compile(
        cpp,
        cpp_path,
        cpp_path[: cpp_path.rfind(".")],
        get_python_filepath(benchmark),
    )
    return exe


def translate_module_for_benchmarking(module: str, benchmark: str) -> str:
    """Remove all the benchmark decorators form a module, translate to C++, and add a main function that times the benchmark function"""
    fnam = module.replace(".", "/") + ".py"
    file = open(fnam).read()
    cpp = pipeline(fnam, file)
    assert "int main" not in cpp
    # Translated code has no literal main() of its own here to call
    # __init_module__() for it (see statement_codegen.py) - this synthetic one
    # has to do it instead, so top-level statements (eg. `SIZE = 30`) run
    # before the benchmark function does.
    main_function = main_function_template(benchmark).replace(
        "int main() {", "int main() {\n        __init_module__();", 1
    )
    cpp += "\n" + main_function
    cpp = "#include <chrono>\n" + cpp
    return cpp


def compile_handwritten_benchmark(benchmark: BenchmarkInfo) -> str:
    filepath = get_handwritten_filepath(benchmark)
    file = open(filepath).read()
    # If there is a previous main function, erase it
    if "int main()" in file:
        file = file[: file.index("int main()")]
    file += "\n" + main_function_template(benchmark.name)
    if "#include <chrono>" not in file:
        file = "#include <chrono>\n" + file
    open(filepath, "w").write(file)
    executable_path = filepath.rstrip(".cpp")
    compile_cpp(filepath, executable_path)
    return executable_path


def main_function_template(benchmark: str) -> str:
    # Add a main function that runs the benchmark and times it
    main_function = """
    int main() {
        auto t0 = std::chrono::steady_clock::now();
        %s(); // Call the benchmarked function
        auto t1 = std::chrono::steady_clock::now();
        auto s = std::chrono::duration<double>(t1 - t0).count() ;
        std::cout << "elapsed: " << s << '\\n';
        return 0;
    }
    """ % benchmark
    return main_function


def import_all() -> None:
    files = glob.glob("microbenchmarks/*.py", root_dir=microbenchmarks_dir.parent)
    files += glob.glob("benchmarks/*.py", root_dir=benchmarks_dir.parent)
    for fnam in files:
        filepath = Path(benchmarks_root_dir / fnam).resolve()
        if filepath.name == "__init__.py" or filepath.suffix != ".py":
            continue
        module_parts = filepath.with_suffix("").relative_to(benchmarks_root_dir).parts
        module = ".".join(module_parts)
        module = "tests.benchmarks." + module
        import_module(module)


def delete_binaries() -> None:
    binaries = [
        p for p in microbenchmarks_dir.iterdir() if p.is_file() and not p.suffix
    ]
    binaries += [p for p in benchmarks_dir.iterdir() if p.is_file() and not p.suffix]
    for fnam in binaries:
        os.remove(fnam)


class Args(NamedTuple):
    benchmark: str


def parse_args() -> Args:
    parser = argparse.ArgumentParser(
        description="Run a mypyc benchmark in compiled and/or interpreted modes."
    )
    parser.add_argument(
        "benchmark",
        nargs="?",
        help="name of benchmark to run (use --list to show options)",
    )
    parser.add_argument(
        "--raw", action="store_true", help="use machine-readable raw output"
    )
    parsed = parser.parse_args()
    if parsed.raw:
        global raw_output
        raw_output = True

    if not parsed.benchmark:
        parser.print_help()
        sys.exit(2)
    args = Args(parsed.benchmark)
    return args


def get_all_benchmarks():
    import_all()
    return benchmarks


def get_benchmark(name: str) -> BenchmarkInfo:
    for benchmark in benchmarks:
        if benchmark.name == name and not benchmark.compiled_variant:
            return benchmark
    sys.exit("unknown benchmark %r" % name)


def main() -> None:

    args = parse_args()

    benchmark = get_benchmark(args.benchmark)

    compiled_times = run_benchmark(benchmark, BenchmarkMode.translated)
    interpreted_times = run_benchmark(benchmark, BenchmarkMode.interpreted)

    # handwritten_times = run_benchmark(benchmark, BenchmarkMode.handwritten)

    stats = []
    stats.append(get_stats("interpreted", interpreted_times))
    stats.append(get_stats("translated", compiled_times))
    # stats.append(get_stats("handwritten", handwritten_times))
    print(tabulate(stats, get_stats_headers()))
    delete_binaries()


def get_stats(label: str, times: list[float]):
    return {
        "label": label,
        "mean": f"{statistics.mean(times):.5f}",
        "cv": f"{100*statistics.stdev(times)/statistics.mean(times):.4f}%",
        "iterations": len(times),
    }


def get_stats_headers():
    return {
        "label": "label",
        "mean": "mean",
        "cv": "cv",
        "iterations": "iterations",
    }


def print_stats(label: str, times: list[float]):
    mean = statistics.mean(times)
    print(
        f"{label}: avg: {statistics.mean(times):.6f}s cv: {100*statistics.stdev(times)/mean:g}% iterations: {len(times)} "
    )


delete_binaries()
# Import before parsing args so that syntax errors get reported.
import_all()

if __name__ == "__main__":
    main()
