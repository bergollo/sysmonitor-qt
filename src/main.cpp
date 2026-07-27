#include "core/systemmonitorworker.h"
#include "ui/mainwindow.h"
#include "ui/statsviewmodel.h"

#include <QApplication>
#include <QDebug>
#include <QQmlContext>
#include <QQuickWidget>
#include <QQuickWindow>
#include <QThread>
#include <QUrl>

int main(int argc, char *argv[])
{
    // QQuickWidget needs a scenegraph backend. Software rendering keeps the
    // reference app usable on headless/forwarded displays without GLX.
    QQuickWindow::setGraphicsApi(QSGRendererInterface::Software);
    QApplication app(argc, argv);

    MainWindow mainWindow;
    StatsViewModel statsModel;
    auto *qmlView = mainWindow.qmlView();
    qmlView->rootContext()->setContextProperty("statsModel", &statsModel);
    QObject::connect(qmlView, &QQuickWidget::statusChanged, qmlView,
                     [qmlView](QQuickWidget::Status status) {
                         if (status != QQuickWidget::Error) {
                             return;
                         }

                         for (const auto &error : qmlView->errors()) {
                             qWarning().noquote() << error.toString();
                         }
                     });
    qmlView->setSource(QUrl("qrc:/qml/Dashboard.qml"));
    // QThread is the event-loop owner; the worker object does the actual work
    // after moveToThread(), which keeps polling out of the GUI event loop.
    QThread workerThread;
    SystemMonitorWorker worker;
    worker.moveToThread(&workerThread);

    QObject::connect(&workerThread, &QThread::started, &worker, &SystemMonitorWorker::start);
    QObject::connect(&worker, &SystemMonitorWorker::statsReady,
                     &mainWindow, &MainWindow::updateStats);
    QObject::connect(&worker, &SystemMonitorWorker::statsReady,
                     &statsModel, &StatsViewModel::updateStats);
    QObject::connect(&worker, &SystemMonitorWorker::finished,
                     &app, &QApplication::quit);
    QObject::connect(&app, &QApplication::aboutToQuit,
                     &workerThread, &QThread::quit);

    mainWindow.show();
    workerThread.start();

    const int result = app.exec();
    workerThread.quit();
    workerThread.wait();
    return result;
}
