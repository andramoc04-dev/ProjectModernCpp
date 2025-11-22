#include "Client.h"
#include <QtWidgets/QApplication>
#include"StartupPage.h"
#include "GamePage.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
   
   Client window;
    window.show();

    StartupPage startup;
    startup.show();
   
    GamePage game;
    game.show();

    
    return app.exec();
}
