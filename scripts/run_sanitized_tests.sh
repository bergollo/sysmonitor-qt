#!/usr/bin/env bash
set -euo pipefail

usage() {
    printf 'Usage: %s <address|undefined|address,undefined|thread> [build-directory]\n' "$0" >&2
}

if [[ $# -lt 1 || $# -gt 2 ]]; then
    usage
    exit 2
fi

sanitizer=$1
case "$sanitizer" in
    address|undefined|address,undefined|thread)
        ;;
    *)
        usage
        exit 2
        ;;
esac

script_dir=$(dirname "$(readlink -f "$0")")
project_root=$(dirname "$script_dir")
build_name=${sanitizer//,/-}
build_dir=${2:-"$project_root/build-sanitize-$build_name"}

cmake -S "$project_root" -B "$build_dir" \
    -DCMAKE_BUILD_TYPE=Debug \
    -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
    -DSYSMONITOR_FETCH_TEST_DEPS=ON \
    -DSYSMONITOR_SANITIZER="$sanitizer"
cmake --build "$build_dir" --parallel

case "$sanitizer" in
    address|address,undefined)
        sanitizer_env=(ASAN_OPTIONS=halt_on_error=1:detect_leaks=1)
        ;;
    undefined)
        sanitizer_env=(UBSAN_OPTIONS=halt_on_error=1:print_stacktrace=1)
        ;;
    thread)
        sanitizer_env=(TSAN_OPTIONS=halt_on_error=1:second_deadlock_stack=1)
        ;;
esac

ctest_args=(--test-dir "$build_dir" --output-on-failure -L sanitizer)
if [[ "$sanitizer" == "thread" ]]; then
    # Qt Quick and platform plugins create many implementation threads. Keep
    # TSan focused on the worker boundary until those external threads are
    # known to be clean with the installed Qt build.
    ctest_args+=(-LE qml)
fi

env "${sanitizer_env[@]}" CTEST_OUTPUT_ON_FAILURE=1 ctest "${ctest_args[@]}"
