#pragma once

#include <QDialog>
#include "ui_Game.h"

class GamePage : public QDialog
{
	Q_OBJECT

public:
	GamePage(QWidget *parent = nullptr);
	~GamePage();

private:
	Ui::GameClass ui;
};

