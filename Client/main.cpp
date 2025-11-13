#include "Client.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    Client window;
    window.show();
    return app.exec();
}
