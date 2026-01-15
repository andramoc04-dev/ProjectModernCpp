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
    explicit MenuPage(QWidget* parent = nullptr);
    MenuPage(int playerId, const QString& playerName, QWidget* parent = nullptr);
    ~MenuPage();

    void setPlayerId(int id);
    int playerId() const;
    void setPlayerName(const QString& name);
    QString getPlayerName() const;
private slots:
    void openProfilePage();
    void OpenLobbyPage();

private:
    Ui::StartupPageClass* ui;
    ProfilePage* profilePage;
    LobbyPage* lobbyPage;
    int m_playerId = -1; 
    QString playerName;
};
