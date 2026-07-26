#include "core/systemmonitorworker.h"
#include "ui/mainwindow.h"
#include "ui/statsviewmodel.h"

#include <QApplication>
#include <QQmlContext>
#include <QQuickWidget>
#include <QQuickWindow>
#include <QUrl>
#include <QThread>

int main(int argc, char *argv[])
{
    QQuickWindow::setGraphicsApi(QSGRendererInterface::Software);
    QApplication app(argc, argv);

    MainWindow mainWindow;
    StatsViewModel statsModel;
    mainWindow.qmlView()->rootContext()->setContextProperty("statsModel", &statsModel);
    mainWindow.qmlView()->setSource(QUrl("qrc:/qml/Dashboard.qml"));
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
