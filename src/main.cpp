#include <QApplication>
#include <QLabel>
#include <QMainWindow>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QMainWindow mainWindow;
    mainWindow.setCentralWidget(new QLabel("Placeholder"));
    mainWindow.resize(640, 480);
    mainWindow.show();

    return app.exec();
}
