# Qt Core and Threading

## QObject Ownership

`QObject` supports a parent-child tree. When a parent is destroyed, Qt deletes
its children. This is why widgets can be created with parents or inserted into
layouts without a separate `delete` in normal UI code.

The worker is different: it must have no parent before `moveToThread()`. A
parented QObject belongs to its parent's ownership tree, and Qt refuses to
move it across threads.

## Signals, Slots, and Queued Delivery

The worker emits a complete `SystemStats` snapshot:

```text
worker.statsReady
        |
        | queued connection
        v
GUI MainWindow / StatsViewModel
```

When sender and receiver live in different threads, Qt posts an event to the
receiver's event loop. The receiver's slot runs in its own thread, not the
sender's thread. This is the key safety property: UI code does not execute on
the polling thread.

## QThread Is an Event-Loop Owner

The worker object, not the `QThread` subclass, contains polling behavior.

```text
main thread
  |
  | creates worker with no parent
  v
worker moved to QThread
  |
  | QThread::started
  v
worker timer starts in worker thread
  |
  | queued statsReady
  v
UI and view-model update in main thread
```

The timer is created in `start()`, after the thread starts. Constructing it in
the main thread would give it the wrong thread affinity.

## Shutdown

`quit()` asks the event loop to stop. `wait()` joins the thread before stack
objects are destroyed. Avoid `terminate()`: it can stop code between resource
operations and bypass normal cleanup.

`QCoreApplication` is enough for non-GUI programs. This application uses
`QApplication` because Widgets and `QQuickWidget` need GUI initialization.

## C++20 Does Not Replace Qt Core

`Q_OBJECT`, signals, slots, and thread affinity are Qt meta-object features,
not standard C++ attributes. The C++20 language upgrade complements them: value
types, `constexpr` policy, and `[[nodiscard]]` improve the boundary, while Qt
still owns the event loop and queued delivery.
