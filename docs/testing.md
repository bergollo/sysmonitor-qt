# Testing

The project uses three levels of verification:

- Unit tests validate parser and CPU math behavior without a display.
- Integration tests can validate QObject signal delivery and view-model state.
- Smoke tests launch the application to catch resource, plugin, and startup
  failures.

The current tests write realistic `/proc` fixtures into `QTemporaryDir`. This
keeps tests deterministic and avoids depending on the host's changing CPU or
memory state. Malformed input and counter edge cases should be tested as
explicit contracts, not inferred from one machine.

Useful next tests:

- `StatsViewModel::statsChanged` with `QSignalSpy`
- history truncation at 60 entries
- worker shutdown and timer ownership
- QML loading without a display
- a fake platform provider for worker tests

Run all registered tests with:

```bash
ctest --test-dir build --output-on-failure
```

Do not make parser unit tests depend on Widgets. A test that needs a GUI event
loop is testing a different boundary and belongs in a separate target.
