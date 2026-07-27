# C++20 Attributes and Compile-Time Properties

This project uses C++ attributes as small, compiler-visible API contracts. An
attribute does not change the algorithm; it tells tools and readers about
intent that would otherwise be easy to miss.

## `[[nodiscard]]`

The Linux readers return `std::optional` because files can be absent or
malformed. Their declarations use diagnostic messages:

```cpp
[[nodiscard("memory parsing can fail and must be handled")]]
std::optional<MemoryInfo> readMemoryInfo(...);
```

Ignoring the result produces a compiler diagnostic. This is appropriate when
the caller almost certainly needs to handle failure. It is not runtime
validation and does not replace a test.

An intentional discard should be explicit:

```cpp
static_cast<void>(readMemoryInfo(path));
```

Do not add `[[nodiscard]]` to every function. A function whose result is merely
convenient to use should not force callers to write ceremony.

## `[[maybe_unused]]`

Use this only when a parameter or local is intentionally unused, commonly in a
platform-specific branch. Do not use it to hide a warning caused by incomplete
implementation. Fix accidental unused values instead.

## `[[likely]]` and `[[unlikely]]`

These are branch-prediction hints, not correctness annotations. A missing
thermal sensor may be unusual, but adding `[[unlikely]]` is not automatically a
performance improvement. Use these only with a measured hot path and a clear
domain reason; this project currently does not need them.

## Other Standard Attributes

- `[[deprecated]]` is useful during an API migration, but there is no obsolete
  public API in this small project yet.
- `[[fallthrough]]` documents intentional switch fallthrough; the project has
  no such switch.
- `[[no_unique_address]]` can optimize an empty policy member, but adding an
  empty policy type solely to demonstrate it would make this design worse.
- `[[assume]]` is not used because an incorrect assumption can create undefined
  behavior and its availability is toolchain-sensitive.

The absence of an attribute can be the correct design decision. Attributes are
metadata, not a replacement for naming, types, tests, or clear control flow.

## Compile-Time Constants

`src/core/monitorconstants.h` owns the polling interval and history limit:

```cpp
inline constexpr int PollIntervalMs = 1000;
inline constexpr std::size_t MaxCpuHistorySamples = 60;
```

`constexpr` means the value can participate in constant evaluation. `inline`
allows the header definition to be included by multiple translation units
without violating the One Definition Rule. The `static_assert` checks make
invalid configuration fail during compilation.

Compare the related keywords:

- `const` prevents mutation after initialization, but the value may be runtime.
- `constexpr` requires a value usable during constant evaluation.
- `consteval` requires every call to be evaluated at compile time.
- `constinit` requires static initialization but does not make an object const.

The project uses `constexpr` because the values are configuration policy, not
because every constant should be forced into compile-time evaluation.

## Defaulted Equality and Designated Initialization

`CpuTimes::operator==` is defaulted so the compiler compares all data members.
Tests use C++20 designated initialization to identify the fields relevant to a
fixture:

```cpp
const CpuTimes snapshot{
    .user = 100,
    .system = 50,
    .idle = 850,
};
```

These features make value-object tests clearer without introducing ownership
or runtime machinery.

## C++ Attributes versus Qt Properties

`[[nodiscard]]` and `[[deprecated]]` are standard C++ attributes processed by
the compiler. `Q_PROPERTY` is a Qt meta-object declaration processed through
Qt's MOC tooling and exposed to QML.

`StatsViewModel` uses:

```cpp
Q_PROPERTY(double cpuPercent READ cpuPercent NOTIFY statsChanged FINAL)
```

`FINAL` means derived Qt meta-object types should not override the property.
`NOTIFY` tells QML when bindings must reevaluate. These are unrelated to the
C++ `final` class specifier and standard attributes, even though the intent is
similar.

Do not add `CONSTANT` to changing statistics. Do not add `REQUIRED` unless QML
construction genuinely requires a property. Use `BINDABLE` only when adopting
Qt's bindable property implementation intentionally.

## Compiler Differences

Attributes can produce different warning wording or severity across GCC,
Clang, and MSVC. The project treats warnings as errors of attention, not as a
reason to suppress diagnostics globally. Verify behavior with the actual native
and ARM64 compilers used for the build.
