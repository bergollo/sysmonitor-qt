#pragma once

#include <QMainWindow>

class QLabel;
class QProgressBar;
class QLineSeries;
class QChartView;
struct SystemStats;

class MainWindow final : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

public slots:
    void updateStats(const SystemStats &stats);

private:
    QProgressBar *cpuBar;
    QProgressBar *memoryBar;
    QLabel *cpuLabel;
    QLabel *memoryLabel;
    QLabel *temperatureLabel;
    QLineSeries *cpuSeries;
    QChartView *chartView;
    int sampleIndex = 0;
};
