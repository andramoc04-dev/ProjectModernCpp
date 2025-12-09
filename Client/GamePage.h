#pragma once

#include <QDialog>
#include <QGridLayout>
#include "ui_GamePage.h"


class GamePage : public QDialog
{
	Q_OBJECT

public:
	GamePage(QWidget *parent = nullptr);
	GamePage(int playerId, int gameId, int opponentId, QWidget* parent = nullptr);
	~GamePage();

	void OnAscendingStack1Clicked();
	void OnAscendingStack2Clicked();
	void OnDescendingStack1Clicked();
	void OnDescendingStack2Clicked();
	void ShowCards(uint16_t number);
private:
	Ui::GameClass ui;
	int m_playerId = -1;
	int m_gameId = -1;
	int m_opponentId = -1;

	void initializeGame(int playerId, int gameId, int opponentId);
};

