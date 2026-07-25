#include <QApplication>
#include <QLabel>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QLabel helloWorld("Hello, World!");
    helloWorld.resize(240, 80);
    helloWorld.show();

    return app.exec();
}
