# Qt Widgets

`MainWindow` is a `QMainWindow` with a central `QTabWidget`. The Widgets tab
contains a metrics group and a `QChartView`; the QML tab contains a
`QQuickWidget`.

Layouts are ownership and geometry managers. Adding a widget to a layout gives
the parent widget responsibility for it and lets Qt calculate sizes when the
window changes. Do not manually position every child unless the UI is a
specialized canvas.

Widget APIs must run on the GUI thread. `MainWindow::updateStats()` is reached
through a queued signal from the worker, so progress bars, labels, and chart
series are safe to update there.

When debugging layout issues, inspect the parent chain, layout margins,
minimum sizes, and size policies before changing arbitrary pixel values.
`QChartView` is useful here because it renders with the Widgets painting path,
separate from the Quick scenegraph problems described in [QML](qml.md).
