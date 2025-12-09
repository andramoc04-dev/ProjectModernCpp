#pragma once

#include <QDialog>
#include "ui_MenuPage.h"
#include "ProfilePage.h"
#include "LobbyPage.h"

namespace Ui { class StartupPageClass; }

class MenuPage : public QDialog
{
    Q_OBJECT

public:
    MenuPage(QWidget* parent = nullptr);
    ~MenuPage();

    void setPlayerId(int id) { m_playerId = id; }
    int playerId() const { return m_playerId; }

private slots:
    void openProfilePage();
    void OpenLobbyPage();

private:
    Ui::StartupPageClass* ui;
    ProfilePage* profilePage;
    LobbyPage* lobbyPage;
    int m_playerId = -1; // id-ul jucătorului - setat după login
};
