#pragma once

#include <QDialog>
#include "ui_MenuPage.h"
#include <QMessageBox>
#include <QLabel>
#include "ProfilePage.h"
#include "LobbyPage.h"

class MenuPage : public QDialog
{
	Q_OBJECT

public:
	MenuPage(QWidget *parent = nullptr);
	~MenuPage();

private:
	Ui::StartupPageClass ui;
	ProfilePage* profilePage;
	
	LobbyPage* lobbyPage;

private slots:
	void openProfilePage();
	void OpenLobbyPage();
};

