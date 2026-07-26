# QML and C++ Integration

QML is declarative: describe what the UI should be as a function of state.
The C++ side exposes state through `StatsViewModel`.

```text
worker.statsReady
        |
        v
StatsViewModel::updateStats()
        |
        v
statsChanged()
        |
        v
QML bindings re-evaluate
```

`Q_PROPERTY` declares readable properties and a notify signal. QML does not
poll the C++ object; it listens for the notify signal and reevaluates only
bindings that depend on changed properties.

`QVariantList` is used for the small CPU history model because it crosses the
Qt/C++/QML boundary easily. A larger application might use a dedicated
`QAbstractListModel` for incremental updates.

The QML file is embedded with `qt_add_resources`. `BASE "qml"` makes the
resource alias predictable, so the source is loaded as:

```text
qrc:/qml/Dashboard.qml
```

`StatCard.qml` is imported from the local `components` directory. Plain
`Text` and `Rectangle` avoid requiring Qt Quick Controls for this small view.

`QQuickWidget` composites Quick content into a Widgets window. The application
uses software rendering to avoid GLX crashes on forwarded/headless displays.
X11 forwarding may still produce washed-out compositing; use VNC or a local
display for visual QML QA. Offscreen execution is for functional smoke tests.
