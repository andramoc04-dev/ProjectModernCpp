#include "Client.h"
#include <QtWidgets/QApplication>
#include"StartupPage.h"
#include "GamePage.h"
#include"LoginPage.h"
int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
   
   Client window;
    window.show();

    StartupPage startup;
    startup.show();
   
    GamePage game;
    game.show();

    LoginPage lg;
    lg.show();
    
    return app.exec();
}
