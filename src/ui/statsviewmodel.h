#pragma once

#include <QObject>
#include <QVariantList>

struct SystemStats;

class StatsViewModel final : public QObject {
    Q_OBJECT
    Q_PROPERTY(double cpuPercent READ cpuPercent NOTIFY statsChanged FINAL)
    Q_PROPERTY(double memoryPercent READ memoryPercent NOTIFY statsChanged FINAL)
    Q_PROPERTY(int memoryUsedMb READ memoryUsedMb NOTIFY statsChanged FINAL)
    Q_PROPERTY(int memoryTotalMb READ memoryTotalMb NOTIFY statsChanged FINAL)
    Q_PROPERTY(QString temperature READ temperature NOTIFY statsChanged FINAL)
    Q_PROPERTY(QVariantList cpuHistory READ cpuHistory NOTIFY statsChanged FINAL)

public:
    explicit StatsViewModel(QObject *parent = nullptr);

    double cpuPercent() const;
    double memoryPercent() const;
    int memoryUsedMb() const;
    int memoryTotalMb() const;
    QString temperature() const;
    QVariantList cpuHistory() const;

public slots:
    // This slot is called through a queued connection on the GUI thread. It
    // translates one immutable C++ snapshot into QML-observable properties.
    void updateStats(const SystemStats &stats);

signals:
    void statsChanged();

private:
    double m_cpuPercent = 0.0;
    double m_memoryPercent = 0.0;
    int m_memoryUsedMb = 0;
    int m_memoryTotalMb = 0;
    QString m_temperature = "unavailable";
    QVariantList m_cpuHistory;
};
