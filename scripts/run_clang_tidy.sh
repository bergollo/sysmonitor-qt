#!/usr/bin/env bash
set -euo pipefail

usage() {
    printf 'Usage: %s <build-directory> [--strict]\n' "$0" >&2
}

if [[ $# -lt 1 || $# -gt 2 ]]; then
    usage
    exit 2
fi

build_dir=$1
strict=${2:-}
compile_commands="$build_dir/compile_commands.json"

if [[ ! -f "$compile_commands" ]]; then
    printf 'Missing %s. Configure with -DCMAKE_EXPORT_COMPILE_COMMANDS=ON.\n' \
        "$compile_commands" >&2
    exit 2
fi

clang_tidy=$(command -v clang-tidy || true)
runner=$(command -v run-clang-tidy.py || command -v run-clang-tidy || true)
if [[ -z "$clang_tidy" ]]; then
    printf 'clang-tidy is required.\n' >&2
    exit 2
fi

printf 'Using %s\n' "$($clang_tidy --version | head -n 1)"

if [[ -n "$runner" ]]; then
    printf 'Using %s\n' "$runner"
    args=(-p "$build_dir" -j "${JOBS:-$(nproc)}" src tests)
    if [[ "$strict" == "--strict" ]]; then
        args+=(--warnings-as-errors='*')
    elif [[ -n "$strict" ]]; then
        usage
        exit 2
    fi
    exec "$runner" "${args[@]}"
fi

printf 'run-clang-tidy.py was not found; analyzing project files sequentially.\n' >&2
mapfile -t sources < <(git ls-files 'src/*.cpp' 'src/**/*.cpp' 'tests/*.cpp')
if [[ ${#sources[@]} -eq 0 ]]; then
    printf 'No project C++ sources were found.\n' >&2
    exit 2
fi

args=(-p "$build_dir")
if [[ "$strict" == "--strict" ]]; then
    args+=(--warnings-as-errors='*')
elif [[ -n "$strict" ]]; then
    usage
    exit 2
fi

for source in "${sources[@]}"; do
    "$clang_tidy" "${args[@]}" "$source"
done
