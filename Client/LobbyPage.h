#pragma once

#include <QDialog>
#include "ui_LobbyPage.h"
#include <QMessageBox>
#include <QLabel>


class LobbyPage : public QDialog
{
	Q_OBJECT

public:
	LobbyPage(QWidget *parent = nullptr);
	~LobbyPage();


private:
	Ui::LobbyPageClass ui;
};

