#include "mainwindow.h"
#include "systemmonitor.h"

#include <QApplication>
#include <QThread>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    MainWindow mainWindow;
    QThread workerThread;
    SystemMonitorWorker worker;
    worker.moveToThread(&workerThread);

    QObject::connect(&workerThread, &QThread::started, &worker, &SystemMonitorWorker::start);
    QObject::connect(&worker, &SystemMonitorWorker::statsReady,
                     &mainWindow, &MainWindow::updateStats);
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
