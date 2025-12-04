#include "MenuPage.h"

MenuPage::MenuPage(QWidget *parent)
	: QDialog(parent), profilePage(nullptr), lobbyPage(nullptr)
{
	ui.setupUi(this);
	connect(ui.profileButton, &QPushButton::clicked, this, &MenuPage::openProfilePage);
    connect(ui.playButton, &QPushButton::clicked, this, &MenuPage::OpenLobbyPage);
}

MenuPage::~MenuPage()
{}


void MenuPage::openProfilePage()
{
    if (!profilePage)
        profilePage = new ProfilePage(this);  

    profilePage->show();   
    this->hide();          
}

void MenuPage::OpenLobbyPage()
{
   
      lobbyPage = new LobbyPage(this);
      lobbyPage->show();
      this->hide();
}

