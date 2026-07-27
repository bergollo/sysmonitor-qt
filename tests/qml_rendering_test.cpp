#include "ui/statsviewmodel.h"
#include "core/systemstats.h"

#include <QApplication>
#include <QColor>
#include <QDebug>
#include <QQmlContext>
#include <QQuickItem>
#include <QQuickWidget>
#include <QQuickWindow>
#include <QSignalSpy>
#include <QTest>
#include <QUrl>

class QmlRenderingTest final : public QObject {
    Q_OBJECT

private slots:
    void loadsDashboardAndCards();
    void updatesViewModelProperties();
};

void QmlRenderingTest::loadsDashboardAndCards()
{
    QQuickWidget view;
    view.setResizeMode(QQuickWidget::SizeRootObjectToView);
    view.resize(640, 480);

    StatsViewModel statsModel(&view);
    view.rootContext()->setContextProperty("statsModel", &statsModel);
    view.setSource(QUrl("qrc:/qml/Dashboard.qml"));

    if (view.status() == QQuickWidget::Error) {
        for (const auto &error : view.errors()) {
            qWarning().noquote() << error.toString();
        }
    }
    QCOMPARE(view.status(), QQuickWidget::Ready);

    auto *root = view.rootObject();
    QVERIFY(root != nullptr);
    QTRY_VERIFY(root->width() > 0.0);
    QTRY_VERIFY(root->height() > 0.0);
    QCOMPARE(root->property("color").value<QColor>(), QColor("#20242b"));

    int cardCount = 0;
    bool foundCpuCard = false;
    for (auto *item : root->findChildren<QQuickItem *>()) {
        if (item->metaObject()->indexOfProperty("title") >= 0) {
            ++cardCount;
            QVERIFY(item->isVisible());
            QVERIFY(item->width() > 0.0);
            QVERIFY(item->height() > 0.0);
            if (item->property("title").toString() == "CPU") {
                foundCpuCard = true;
                QCOMPARE(item->property("valueText").toString(), QString("0.0%"));
            }
        }
    }
    QCOMPARE(cardCount, 3);
    QVERIFY(foundCpuCard);
}

void QmlRenderingTest::updatesViewModelProperties()
{
    StatsViewModel statsModel;
    QSignalSpy statsChanged(&statsModel, &StatsViewModel::statsChanged);
    const SystemStats stats{
        .cpuPercent = 42.5,
        .memory = MemoryInfo{.totalKb = 4096000, .availableKb = 2048000},
        .temperatureCelsius = 55.0,
    };

    statsModel.updateStats(stats);

    QCOMPARE(statsChanged.count(), 1);
    QCOMPARE(statsModel.cpuPercent(), 42.5);
    QCOMPARE(statsModel.memoryPercent(), 50.0);
    QCOMPARE(statsModel.memoryUsedMb(), 2000);
    QCOMPARE(statsModel.temperature(), QString("55.0 C"));
    QCOMPARE(statsModel.cpuHistory().size(), 1);
}

int main(int argc, char *argv[])
{
    QQuickWindow::setGraphicsApi(QSGRendererInterface::Software);
    QApplication app(argc, argv);
    QmlRenderingTest test;
    return QTest::qExec(&test, argc, argv);
}

#include "qml_rendering_test.moc"
