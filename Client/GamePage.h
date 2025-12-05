#pragma once

#include <QDialog>
#include "ui_GamePage.h"


class GamePage : public QDialog
{
	Q_OBJECT

public:
	GamePage(QWidget *parent = nullptr);
	~GamePage();

	void OnAscendingStack1Clicked();
	void OnAscendingStack2Clicked();
	void OnDescendingStack1Clicked();
	void OnDescendingStack2Clicked();
	void ShowButton(uint16_t number);
private:
	Ui::GameClass ui;

};

