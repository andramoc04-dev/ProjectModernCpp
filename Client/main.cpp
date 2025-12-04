#include "Client.h"
#include <QtWidgets/QApplication>
#include"StartupPage.h"
#include "GamePage.h"
#include"LoginPage.h"
#include "MinigamePage.h"
int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
   
    Client window;
    window.show();

    GamePage game;
    game.show();

    MinigamePage minigame;
    minigame.show();

    StartupPage startup;
    startup.show();
    
    
    return app.exec();
}
