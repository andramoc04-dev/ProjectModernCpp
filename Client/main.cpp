#include "Client.h"
#include <QtWidgets/QApplication>
#include "LoginPage.h"
#include"StartupPage.h"
int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
   
    Client window;
    window.show();

    LoginPage login;
    login.show();

    StartupPage startup;
    startup.show();
    
    return app.exec();
}
