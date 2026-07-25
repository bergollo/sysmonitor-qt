#include "mainwindow.h"

#include "systemmonitor.h"

#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QProgressBar>
#include <QTabWidget>
#include <QVBoxLayout>
#include <QQuickWidget>

#include <QtCharts/QChart>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , cpuBar(new QProgressBar)
    , memoryBar(new QProgressBar)
    , cpuLabel(new QLabel("CPU: waiting for data"))
    , memoryLabel(new QLabel("Memory: waiting for data"))
    , temperatureLabel(new QLabel("Temperature: unavailable"))
    , cpuSeries(new QLineSeries)
    , chartView(new QChartView)
    , qmlDashboard(new QQuickWidget)
{
    setWindowTitle("System Monitor");
    resize(720, 520);

    cpuBar->setRange(0, 100);
    memoryBar->setRange(0, 100);

    auto *metrics = new QGroupBox("Current readings");
    auto *metricsLayout = new QGridLayout(metrics);
    metricsLayout->addWidget(cpuLabel, 0, 0);
    metricsLayout->addWidget(cpuBar, 0, 1);
    metricsLayout->addWidget(memoryLabel, 1, 0);
    metricsLayout->addWidget(memoryBar, 1, 1);
    metricsLayout->addWidget(temperatureLabel, 2, 0, 1, 2);

    auto *chart = new QChart;
    chart->addSeries(cpuSeries);
    chart->setTitle("CPU usage history");
    auto *axisX = new QValueAxis;
    auto *axisY = new QValueAxis;
    axisX->setRange(0, 60);
    axisY->setRange(0, 100);
    chart->addAxis(axisX, Qt::AlignBottom);
    chart->addAxis(axisY, Qt::AlignLeft);
    cpuSeries->attachAxis(axisX);
    cpuSeries->attachAxis(axisY);
    chartView->setChart(chart);
    chartView->setMinimumHeight(260);

    auto *widgetsPage = new QWidget;
    auto *layout = new QVBoxLayout(widgetsPage);
    layout->addWidget(metrics);
    layout->addWidget(chartView);

    auto *tabs = new QTabWidget;
    tabs->addTab(widgetsPage, "Widgets");
    tabs->addTab(qmlDashboard, "QML");
    setCentralWidget(tabs);
}

QQuickWidget *MainWindow::qmlView() const
{
    return qmlDashboard;
}

void MainWindow::updateStats(const SystemStats &stats)
{
    const auto usedMemoryKb = stats.memory.totalKb - stats.memory.availableKb;
    const auto memoryPercent = 100.0 * static_cast<double>(usedMemoryKb)
        / static_cast<double>(stats.memory.totalKb);

    cpuBar->setValue(static_cast<int>(stats.cpuPercent));
    memoryBar->setValue(static_cast<int>(memoryPercent));
    cpuLabel->setText(QString("CPU: %1%").arg(stats.cpuPercent, 0, 'f', 1));
    memoryLabel->setText(QString("Memory: %1/%2 MiB")
                             .arg(usedMemoryKb / 1024)
                             .arg(stats.memory.totalKb / 1024));
    temperatureLabel->setText(stats.temperatureCelsius
                                  ? QString("Temperature: %1 C")
                                        .arg(*stats.temperatureCelsius, 0, 'f', 1)
                                  : "Temperature: unavailable");

    cpuSeries->append(sampleIndex++, stats.cpuPercent);
    if (cpuSeries->count() > 60) {
        cpuSeries->remove(0);
    }
    const auto axes = chartView->chart()->axes(Qt::Horizontal);
    if (!axes.isEmpty()) {
        axes.first()->setRange(qMax(0, sampleIndex - 60), qMax(60, sampleIndex));
    }
}
