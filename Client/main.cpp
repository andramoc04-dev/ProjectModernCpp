#include "Client.h"
#include <QtWidgets/QApplication>
#include"StartupPage.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
   
   Client window;
    window.show();

    StartupPage startup;
    startup.show();
   
    
    return app.exec();
}
