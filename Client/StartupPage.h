#pragma once

#include <QDialog>
#include "ui_StartupPage.h"
#include <QMessageBox>
#include <QLabel>
#include "ProfilePage.h"
#include "LoginPage.h"
#include "LobbyPage.h"

class StartupPage : public QDialog
{
	Q_OBJECT

public:
	StartupPage(QWidget *parent = nullptr);
	~StartupPage();

private:
	Ui::StartupPageClass ui;
	ProfilePage* profilePage;
	LoginPage* loginPage;
	LobbyPage* lobbyPage;

private slots:
	void openProfilePage();
	void OpenLoginPageOrLobbyPage();
};

