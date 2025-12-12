#include "MenuPage.h"
#include "ui_MenuPage.h" 
#include <QMessageBox>

MenuPage::MenuPage(QWidget* parent)
    : QDialog(parent), ui(new Ui::StartupPageClass), profilePage(nullptr), lobbyPage(nullptr), m_playerId(-1)
{
    ui->setupUi(this);
    connect(ui->profileButton, &QPushButton::clicked, this, &MenuPage::openProfilePage);
    connect(ui->playButton, &QPushButton::clicked, this, &MenuPage::OpenLobbyPage);
}

MenuPage::~MenuPage()
{
}

void MenuPage::openProfilePage()
{
    if (!profilePage)
        profilePage = new ProfilePage(this);

    profilePage->show();
    this->hide();
}




void MenuPage::OpenLobbyPage()
{
    if (m_playerId < 0) {
        QMessageBox::warning(this, "Nu ești logat", "Trebuie să te autentifici înainte de a intra în lobby.");
        return;
    }

    QString serverUrl = "http://127.0.0.1:18080";
    lobbyPage = new LobbyPage(m_playerId,playerName, serverUrl, this);
    lobbyPage->show();
    this->hide();
}
