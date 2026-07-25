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
    std::optional<CpuTimes> previousCpuTimes;
    QTimer *timer = nullptr;
};
