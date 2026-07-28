# Static Analysis

Static analysis complements compilation and tests. The compiler catches invalid
programs, GoogleTest and QtTest check behavior, clang-tidy reviews modern C++
patterns, and Clazy reviews Qt-specific usage.

## Tool Boundaries

- Compiler warnings are always enabled through `cmake/CompilerWarnings.cmake`.
- `clang-tidy` checks C++ correctness, bug patterns, selected modernization,
  performance, and readability concerns.
- `clazy-standalone` checks Qt idioms such as QObject ownership, signal/slot
  usage, Qt containers, and unnecessary Qt API work.
- The normal application build does not require either analyzer.

Clang-tidy and Clazy are intentionally separate. Clang-tidy is a general C++
tool that reads a compilation database. Clazy is a Qt-aware Clang-based tool
and needs Qt headers and compile options. Running them separately makes each
diagnostic easier to understand and keeps third-party code out of the report.

## Installation

On Ubuntu, install the tools and normal project dependencies with:

```bash
sudo apt install cmake build-essential clang-tidy clang-tools clazy \
    qt6-base-dev qt6-charts-dev qt6-declarative-dev
```

Print the installed versions before interpreting a new baseline:

```bash
clang-tidy --version
clazy-standalone --version
```

Tool versions are not embedded in the application binary. Review analyzer
updates deliberately because a newer LLVM or Clazy release can add diagnostics.

## clang-tidy

The repository configuration is in `.clang-tidy`. It starts with focused checks:

- `bugprone-*`
- `clang-analyzer-*`
- selected safe `modernize-*` checks
- `performance-*`
- selected readability checks

Broad naming, Google-style, and all C++ Core Guidelines checks are not enabled
yet. They are useful learning material, but enabling them all at once would mix
style migration with correctness work and create a noisy baseline.

Configure a dedicated build with a compilation database:

```bash
cmake -S . -B build-tidy \
    -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
cmake --build build-tidy --parallel
./scripts/run_clang_tidy.sh build-tidy
```

Use strict mode when the current baseline is clean:

```bash
./scripts/run_clang_tidy.sh build-tidy --strict
```

CMake also supports target-scoped analysis during compilation:

```bash
cmake -S . -B build-tidy-cmake \
    -DSYSMONITOR_ENABLE_CLANG_TIDY=ON \
    -DSYSMONITOR_CLANG_TIDY_STRICT=ON
cmake --build build-tidy-cmake --parallel
```

This option is useful for learning how CMake attaches tools to targets. The
standalone runner is preferred for CI because it gives explicit control over
which source directories are analyzed.

## Clazy

Clazy starts at level 1 because those checks are generally safe and actionable:

```bash
cmake -S . -B build-clazy \
    -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
cmake --build build-clazy --parallel
./scripts/run_clazy.sh build-clazy level1
```

Level 2 is more opinionated and should be evaluated separately:

```bash
./scripts/run_clazy.sh build-clazy level2
```

Do not use a Clazy compiler wrapper in the ordinary `build/` directory. If a
platform only provides the wrapper and not `clazy-standalone`, configure a fresh
build with that compiler and keep it separate from native and ARM64 builds.

## Fixes and Suppressions

Do not run automatic fixes directly on the working tree. Export or inspect the
proposed changes, then apply them in a focused commit and run the full build and
tests afterward.

Prefer fixing the code over suppressing a diagnostic. When suppression is the
correct choice, make it narrow and explain the reason:

```cpp
// NOLINTNEXTLINE(check-name): Qt API requires this intentional conversion.
someQtCall(value);
```

Never add a repository-wide `NOLINT` blanket or disable a whole analyzer solely
to hide an unexplained finding. Exclude generated files, system headers, and
third-party sources through configuration instead.

## Learning Progression

1. Run the tools without strict mode and read every diagnostic.
2. Fix correctness and ownership findings before style suggestions.
3. Study one modernization check at a time and compare the generated fix with
   the C++20 material in this project.
4. Review Clazy findings against Qt object ownership and thread-affinity rules.
5. Enable strict mode only after the baseline is understood and clean.
6. Re-run the complete build, CTest suite, and headless smoke test after fixes.

Static analysis is not a replacement for tests. A clean analyzer run does not
prove parser behavior, signal delivery, QML bindings, or safe thread shutdown.
