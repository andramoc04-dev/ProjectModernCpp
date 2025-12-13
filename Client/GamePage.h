#pragma once

#include <QDialog>
#include <QGridLayout>
#include <QNetworkAccessManager>
#include <QTimer>
#include "ui_GamePage.h"




class GamePage : public QDialog
{
	Q_OBJECT

public:
	GamePage(QWidget *parent = nullptr);
	GamePage(int playerId,const QString& playerName, int gameId, int opponentId, QWidget* parent = nullptr);
	~GamePage();

	void OnAscendingStack1Clicked();
	void OnAscendingStack2Clicked();
	void OnDescendingStack1Clicked();
	void OnDescendingStack2Clicked();
	void ShowCards(uint16_t number);
	void shuffleDeck();
	
private slots:
	void onSendClicked();
	void fetchMessages();

private:
	Ui::GameClass ui;
	int m_playerId = -1;
	int m_gameId = -1;
	int m_opponentId = -1;

	QNetworkAccessManager* net;
	QTimer* chatTimer;

	QString m_playerName; // setat după login

	void initializeGame(int playerId,const QString& playerName, int gameId, int opponentId);
};

