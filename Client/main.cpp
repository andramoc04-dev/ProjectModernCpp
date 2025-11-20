#include "Client.h"
#include <QtWidgets/QApplication>
#include "LoginPage.h"
#include"StartupPage.h"
#include "LobbyPage.h"
int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
   
    Client window;
    window.show();

    LoginPage login;
    login.show();

    StartupPage startup;
    startup.show();
    
    LobbyPage lobby;
    lobby.show();
    return app.exec();
}
