#include "MenuPage.h"
#include "ui_MenuPage.h" 
#include <QMessageBox>

MenuPage::MenuPage(QWidget* parent)
    : MenuPage(-1, "", parent)
{
}

MenuPage::MenuPage(int playerId, const QString& playerName, QWidget* parent)
    : QDialog(parent),
    ui(new Ui::StartupPageClass),
    m_playerId(playerId),
    playerName(playerName)
{
    ui->setupUi(this);

    connect(ui->profileButton, &QPushButton::clicked, this, &MenuPage::openProfilePage);
    connect(ui->playButton, &QPushButton::clicked, this, &MenuPage::OpenLobbyPage);
}

MenuPage::~MenuPage()
{
}

void MenuPage::setPlayerId(int id)
{
     m_playerId = id; 
}

int MenuPage::playerId() const
{
    return m_playerId;
}

void MenuPage::setPlayerName(const QString& name)
{
    playerName = name;
}

QString MenuPage::getPlayerName() const
{
    return playerName;
}

void MenuPage::openProfilePage()
{
    if (!profilePage)
        profilePage = new ProfilePage(m_playerId, playerName, "http://127.0.0.1:18080", nullptr);
    profilePage->show();
    this->close();

}
void MenuPage::OpenLobbyPage()
{
    if (m_playerId < 0) {
        QMessageBox::warning(this, "You are not logged in.", "You must log in before entering the lobby.");
        return;
    }

    const QString serverUrl = "http://127.0.0.1:18080";
    lobbyPage = new LobbyPage(m_playerId,playerName, serverUrl, nullptr);
    lobbyPage->show();
    this->close();
}
