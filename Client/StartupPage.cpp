#include "StartupPage.h"

StartupPage::StartupPage(QWidget *parent)
	: QDialog(parent), profilepage(nullptr)
{
	ui.setupUi(this);
	connect(ui.profileButton, &QPushButton::clicked, this, &StartupPage::openProfilePage);
    connect(ui.playButton, &QPushButton::clicked, this, &StartupPage::OpenLoginPageOrLobbyPage);
}

StartupPage::~StartupPage()
{}


void StartupPage::openProfilePage()
{
    if (!profilepage)
        profilepage = new ProfilePage(this);  

    profilepage->show();   
    this->hide();          
}

void StartupPage::OpenLoginPageOrLobbyPage()
{
    if (!loginPage)     
    {
        loginPage = new LoginPage(this);
        loginPage->show();
        this->hide();
    }
    else
        if(loginPage->getLoggedIn()==false)
        {
            loginPage->show();
            this->hide();
        }
        else
            {
                lobbyPage = new LobbyPage(this);
                lobbyPage->show();
                this->hide();
            }
}

