# Testing

The project uses three levels of verification and two complementary test
frameworks:

- GoogleTest unit tests validate parser, value-type, and CPU math behavior
  without a display. Fixtures and parameterized tests make input contracts
  visible while keeping these tests independent of Qt's event loop.
- QtTest integration tests validate QObject signal delivery, worker lifecycle,
  view-model state, and QML loading. `QSignalSpy` and `QTRY_*` are deliberately
  kept for Qt-facing behavior rather than duplicated with another framework.
- Smoke tests launch the application to catch resource, plugin, and startup
  failures.

This split is intentional: GoogleTest is a widely used modern C++ testing API,
while QtTest understands QObject metadata, signals, event loops, and Qt Quick.
Adding both GoogleTest and Catch2 would duplicate the general-purpose layer
without improving coverage or the learning path.

The current GoogleTest tests write realistic `/proc` fixtures into temporary
directories using the standard filesystem library. This keeps tests
deterministic and avoids depending on the host's changing CPU or memory state.
Malformed input and counter edge cases should be tested as explicit contracts,
not inferred from one machine.

Useful next tests:

- `StatsViewModel::statsChanged` with `QSignalSpy`
- history truncation at 60 entries
- worker shutdown and timer ownership
- QML loading without a display
- a fake platform provider for worker tests

## CMake and Dependencies

GoogleTest is pinned to the `v1.17.0` commit in `CMakeLists.txt`. By default,
CMake first uses an installed `GTest` package and fetches the pinned source only
when it is unavailable. Set `-DSYSMONITOR_FETCH_TEST_DEPS=OFF` to require a
system package, which is useful for offline or distribution builds.

The GoogleTest target uses `gtest_discover_tests()`, so every test case and
parameterized case appears individually in CTest. The QtTest targets remain
single executables because QtTest owns their test-case discovery internally.

Run all registered tests with:

```bash
ctest --test-dir build --output-on-failure
```

Useful focused runs:

```bash
ctest --test-dir build -L unit --output-on-failure
ctest --test-dir build -R QmlRenderingTests --output-on-failure
```

Do not make parser unit tests depend on Widgets. A test that needs a GUI event
loop is testing a different boundary and belongs in a separate target.

Compiler attributes are verified at compile time rather than with runtime
assertions. Runtime tests should instead verify the behavior the attribute
protects: parser callers handle `std::optional`, and view-model updates emit
`statsChanged` and expose the expected property values. The QML rendering test
covers resource loading, the property-binding boundary, missing sensor
presentation, and bounded history behavior.

Static analysis is documented separately in [Static analysis](static-analysis.md).
