# Exercises

These are intentionally short design exercises for a senior engineer. First
predict the failure mode, then implement and test the smallest change.

## C++

- Add a `constexpr` history limit and test it.
- Extract CPU snapshot formatting into a pure function.
- Add tests for decreasing counters and zero elapsed time.
- Compare `std::optional` with a result type carrying an error message.
- Explain why `std::move` would be unnecessary for `SystemStats` here.

## Qt

- Add a worker error signal with a descriptive message.
- Add a configurable polling interval.
- Use `QSignalSpy` to test view-model notifications.
- Add a pause/resume command without sharing mutable state.
- Verify receiver thread affinity in a test.

## QML

- Add a warning color when temperature exceeds a threshold.
- Add a “no data yet” state.
- Add a history visualization using the existing list.
- Replace the context property with a registered QML type and compare the
  lifecycle tradeoffs.

## CMake

- Add Debug/Release presets.
- Add optional AddressSanitizer flags.
- Add a formatting target using `.clang-format`.
- Add QML deployment to the install tree.
