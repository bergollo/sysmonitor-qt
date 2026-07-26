#pragma once

#include "../core/systemstats.h"

#include <QObject>
#include <optional>

class QTimer;

class SystemMonitorWorker final : public QObject {
    Q_OBJECT

public slots:
    void start();

signals:
    void statsReady(const SystemStats &stats);
    void finished();

private slots:
    void poll();

private:
    // The worker has no parent: QObject ownership and thread affinity are
    // separate, and moveToThread() cannot move an already parented object.
    std::optional<CpuTimes> previousCpuTimes;
    QTimer *timer = nullptr;
};
