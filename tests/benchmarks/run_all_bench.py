from __future__ import annotations

import argparse
import glob
import os
import re
import statistics
import subprocess
import sys
import time
from enum import Enum, auto
from importlib import import_module
from pathlib import Path
from typing import NamedTuple, Optional

from tqdm import tqdm
from typing_extensions import Final

from tests.benchmarks import record_bench
from tests.benchmarks.benchmarking import BenchmarkInfo, benchmarks
from tests.benchmarks.run_bench import (
    BenchmarkMode,
    get_all_benchmarks,
    import_all,
    run_benchmark,
    run_interpreted,
)

excluded_files = {
    "bytes",
    "enums",
    "callables",
    "dataclasses",
    "instances",
    "bm_deltablue",
    "bm_hexiom",
    "bm_nqueens",
    "bm_raytrace",
    "bm_richards",
    "bm_spectral_norm",
    "bm_float",
}

# excluded_files = {
#     "bytes",
#     "enums",
#     "callables",
#     "dataclasses",
#     "instances",
#     "binary_trees",
#     "bm_deltablue",
#     "bm_hexiom",
#     "bm_nqueens",
#     "bm_raytrace",
#     "bm_richards",
#     "bm_spectral_norm",
#     "bm_float",
# }
#


def main():
    # record_python_baselines()
    args = parse_args()
    if args.is_list:
        list_benchmarks()
        sys.exit(2)
    find_failing_translations()
    pass


def list_benchmarks():
    for benchmark in get_all_benchmarks_filtered():
        file_name = benchmark.module.split(".")[-1]
        suffix = ""
        if "microbenchmarks" in benchmark.module:
            suffix += " (micro)"
        suffix += f" [{file_name}]"
        print(benchmark.name + suffix)


def get_all_benchmarks_filtered():
    benchmarks = get_all_benchmarks()
    filtered_benchmarks = []
    for benchmark in benchmarks:
        file_name = benchmark.module.split(".")[-1]
        if file_name in excluded_files:
            continue
        filtered_benchmarks.append(benchmark)
    return filtered_benchmarks


def find_failing_translations():
    benchmarks = get_all_benchmarks_filtered()
    benchmark_map = {benchmark.name: benchmark for benchmark in benchmarks}
    for benchmark in tqdm(benchmark_map):
        try:
            run_benchmark(benchmark_map[benchmark], BenchmarkMode.translated)
        except Exception as e:
            print(e)


def record_python_baselines():
    benchmarks = get_all_benchmarks()
    benchmark_map = {benchmark.name: benchmark for benchmark in benchmarks}
    for benchmark in tqdm(benchmark_map):
        times = run_benchmark(
            benchmark_map[benchmark], BenchmarkMode.interpreted, min_iter=20
        )
        record_bench.write_to_csv(
            benchmark, BenchmarkMode.interpreted, times, override_save=True
        )


class Args(NamedTuple):
    is_list: bool


def parse_args() -> Args:
    parser = argparse.ArgumentParser(
        description="Run a mypyc benchmark in compiled and/or interpreted modes."
    )
    parser.add_argument(
        "--list", action="store_true", help="show names of all benchmarks"
    )
    parsed = parser.parse_args()
    args = Args(parsed.list)
    return args


if __name__ == "__main__":
    main()
