#include "Client.h"
#include <QtWidgets/QApplication>
#include"MenuPage.h"
#include "GamePage.h"
#include"LoginPage.h"
#include "MinigamePage.h"
int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    GamePage game;
    game.show();

    MinigamePage minigame;
    minigame.show();

    LoginPage login;
    login.show();
    
    
    return app.exec();
}
