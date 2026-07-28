#!/usr/bin/env bash
set -euo pipefail

usage() {
    printf 'Usage: %s <build-directory> [level1|level2]\n' "$0" >&2
}

if [[ $# -lt 1 || $# -gt 2 ]]; then
    usage
    exit 2
fi

build_dir=$1
level=${2:-level1}
compile_commands="$build_dir/compile_commands.json"

if [[ ! -f "$compile_commands" ]]; then
    printf 'Missing %s. Configure with -DCMAKE_EXPORT_COMPILE_COMMANDS=ON.\n' \
        "$compile_commands" >&2
    exit 2
fi

if [[ "$level" != level1 && "$level" != level2 ]]; then
    usage
    exit 2
fi

clazy=$(command -v clazy-standalone || true)
if [[ -z "$clazy" ]]; then
    printf 'clazy-standalone is required for compilation-database analysis.\n' >&2
    printf 'Install the Clazy standalone package or use a Clazy compiler build.\n' >&2
    exit 2
fi

printf 'Using %s\n' "$($clazy --version 2>&1 | head -n 1)"
printf 'Running Clazy %s checks on project sources.\n' "$level"

mapfile -t sources < <(git ls-files 'src/*.cpp' 'src/**/*.cpp' 'tests/*.cpp')
if [[ ${#sources[@]} -eq 0 ]]; then
    printf 'No project C++ sources were found.\n' >&2
    exit 2
fi

exec "$clazy" -p "$compile_commands" -checks="$level" "${sources[@]}"
