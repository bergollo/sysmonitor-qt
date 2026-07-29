# Dynamic Analysis

Dynamic analysis runs the program and tests with instrumented compiler/runtime
checks. It complements compiler warnings, clang-tidy, Clazy, GoogleTest, and
QtTest; no single tool proves that a threaded GUI application is correct.

## Sanitizers

- **AddressSanitizer (ASan)** detects out-of-bounds access, use-after-free,
  double-free, and related memory errors. Leak detection is enabled by default
  in the runner.
- **UndefinedBehaviorSanitizer (UBSan)** detects selected undefined behavior,
  including invalid arithmetic, casts, alignment, and unreachable assumptions.
- **ThreadSanitizer (TSan)** detects data races and some synchronization errors.
  It must run in a separate build from ASan and UBSan.

The project supports `address`, `undefined`, `address,undefined`, and `thread`
through `SYSMONITOR_SANITIZER`. Sanitizer builds use GCC or Clang on the native
Linux build. They are not automatically applied to ARM64 cross-builds because
runtime/compiler support must be verified separately for the target sysroot.

## Running Locally

The helper creates a dedicated Debug build directory, instruments the project,
and runs the tests labelled `sanitizer`:

```bash
./scripts/run_sanitized_tests.sh address
./scripts/run_sanitized_tests.sh undefined
./scripts/run_sanitized_tests.sh thread
```

The combined memory/undefined mode is also available:

```bash
./scripts/run_sanitized_tests.sh address,undefined
```

Use a second argument to select a custom build directory. Never reuse `build/`,
`build-aarch64/`, or a static-analysis build for sanitizer experiments.

## Test Scope

ASan and UBSan run the parser, worker, and headless QML tests. TSan runs the
parser and worker tests but excludes QML. Qt Quick and platform plugins create
implementation threads outside this project, so TSan results from those tests
would initially contain external noise rather than teach the worker's actual
threading contract.

The worker tests deliberately use a real `QThread`, queued `statsReady` delivery,
thread identity checks, and `quit()` followed by `wait()`. This gives TSan a
meaningful synchronization boundary to inspect instead of only testing a worker
by direct method calls.

## Runtime Options

The runner uses fail-fast settings with stack traces:

```text
ASAN_OPTIONS=halt_on_error=1:detect_leaks=1
UBSAN_OPTIONS=halt_on_error=1:print_stacktrace=1
TSAN_OPTIONS=halt_on_error=1:second_deadlock_stack=1
```

Do not disable leak detection globally to hide a failure. If a prebuilt Qt or
platform plugin produces a confirmed external leak, isolate and document that
exception rather than suppressing project frames.

## Interpreting Reports

1. Read the first project-owned stack frame, not only the first runtime frame.
2. Re-run the smallest failing test directly.
3. Add or improve a focused test that demonstrates the contract.
4. Fix ownership, lifetime, arithmetic, or synchronization in production code.
5. Re-run the affected sanitizer and the normal CTest suite.

Sanitizer suppressions should be rare, narrow, and external. Do not add a broad
environment suppression or disable an entire sanitizer to make CI pass.

## CI

GitHub Actions runs separate native jobs for ASan, UBSan, and TSan. Each matrix
entry gets a fresh build directory and uses the same runner as local development.
The ordinary build/test, static-analysis, and sanitizer jobs remain separate so
failures identify the layer that needs attention.
