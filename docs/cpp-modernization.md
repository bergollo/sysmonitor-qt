# Modern C++ Refresher

The code now uses C++20, building on C++17 features. The important shift from
C++11 is not syntax:
it is making ownership, failure, and value boundaries explicit.

## Ownership and RAII

An object with automatic storage is destroyed when its scope ends. This is
RAII: resource acquisition is initialization, and destruction is deterministic.
`QThread workerThread;` in `main()` is an example. It is not leaked and its
lifetime is bounded by `main()`.

Qt adds a second ownership system: a `QObject` parent destroys its children.
Do not confuse that with C++ ownership. `new QTimer(this)` is safe because
the worker owns the timer through Qt's parent-child tree.

## Values, References, and Pointers

`SystemStats` is passed by `const` reference inside the process to avoid a
needless copy while preventing mutation. `statsReady` still communicates a
snapshot across threads. Qt copies signal arguments for a queued connection,
which is safer than sharing mutable fields.

Use a pointer when an object may be absent, has identity, or is owned elsewhere.
Use a reference when absence is impossible and the callee does not take
ownership. Use a value for small independent data.

## `std::optional`

`std::optional<T>` represents either a valid `T` or no value. Parser functions
use it for missing files and malformed input instead of returning `0`, which
could be mistaken for a real CPU counter or temperature.

```cpp
const auto memory = readMemoryInfo();
if (!memory) {
    return std::nullopt;
}
```

## Lambdas and Captures

Qt connections frequently use lambdas for small adapters. A capture such as
`[&]` borrows local variables, so its lifetime must not outlive the scope. A
QObject connection can disconnect automatically, but that does not make a
reference capture safe after the referenced stack object is gone.

## Filesystem and Error Handling

`std::filesystem` expresses paths and directory iteration portably. The
thermal reader uses `std::error_code` because a missing thermal directory is
an expected device condition, not necessarily an exceptional programming bug.

## Headers and Translation Units

Declarations belong in headers; definitions belong in `.cpp` files. Include
what you use rather than relying on transitive includes. Forward declarations
reduce compile-time dependencies when only a pointer or reference is needed.
The One Definition Rule is why non-inline function definitions must not be
duplicated across translation units.

## Integer Width and Overflow

`std::uint64_t` documents that `/proc` counters are non-negative integer
quantities. Arithmetic can still overflow if a value exceeds the type range;
the current project assumes normal kernel counter behavior and validates
backwards deltas before calculating percentages.

## API Boundaries

The core layer uses standard C++ types and has no Widgets dependency. That is
intentional: it makes the parser and math testable without a display and keeps
the domain boundary understandable. Qt is introduced at the worker boundary,
where an event loop and signals are actually needed.

## Standard-Version Comparison

The detailed feature timeline is in [C++11, C++17, and C++20](cpp11-17-20.md).
In this repository, the practical progression is:

| Need | C++11 tendency | C++17 improvement | C++20 option |
|---|---|---|---|
| Missing parser value | Sentinel or pointer | `std::optional` | Same, with `[[nodiscard]]` to catch ignored failures |
| Generic constraints | SFINAE and traits | `if constexpr` | Concepts and `requires` |
| Collection access | Iterator pairs | `std::string_view` | `std::span` and ranges |
| Thread cancellation | Custom flags | Qt signals or futures | Stop tokens and `std::jthread` where Qt is not the event-loop owner |
| Formatting | Streams | Qt formatting or conversion helpers | `std::format` where target library support is acceptable |

The project uses C++20 designated initialization and defaulted equality in test
fixtures, plus `[[nodiscard]]` on parser APIs. It does not use modules,
coroutines, or `std::jthread` because those would add complexity without
improving this Qt event-driven design.

## Migration Hazards

- A newer language standard does not make old ownership mistakes safe.
- `std::span` and `std::string_view` do not own their referenced storage.
- Ranges can hide lifetime problems if a view escapes its source scope.
- `std::jthread` and `QThread` have different event-loop and signal semantics.
- C++20 library availability depends on the compiler and standard library, not
  only the compiler front-end version.
- Cross-compilers and target sysroots must support the same selected features.
