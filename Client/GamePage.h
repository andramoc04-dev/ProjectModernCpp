#pragma once

#include <QDialog>
#include <QGridLayout>
#include <QNetworkAccessManager>
#include <QTimer> 
#include "../TimerDLL/GameTimer.h"
#include "ui_GamePage.h"
#include <thread>
#include <atomic>
#include "EndgamePage.h"




class GamePage : public QDialog
{
	Q_OBJECT

public:
	GamePage(QWidget *parent = nullptr);
	GamePage(int playerId,const QString& playerName, int gameId, int opponentId, QWidget* parent = nullptr);
	~GamePage();

	void FetchHand();
	void ShowCards(const std::vector<int>& cardsValues);
	void shuffleDeck();
	void DistributeCards();
	void updatePileVisual(QLabel* pileLabel, int cardValue);
	void fetchGameState();
	void showPowerUps(const std::vector<int>& equippedPowerUpIds);
	void fetchEquippedPowerUps();
private slots:
	void onSendClicked();
	void fetchMessages();
	void onPileClicked(int pileId, QLabel* pileLabel);
	void onDrawCardClicked();
	void checkTimers();
	void openEndgamePage(QString gameStatus);

private:
	Ui::GameClass ui;
	int m_playerId = -1;
	int m_gameId = -1;
	int m_opponentId = -1;
	int m_selectedCardValue = -1;
	int m_cardsPlacedThisTurn = 0;
	QStringList m_allPlayerNames;


	GameTimer m_chatTimer;
	GameTimer m_stateTimer;

	QTimer* m_pollTimer;

	QNetworkAccessManager* net;
	

	QString m_playerName; 

	void initializeGame(int playerId,const QString& playerName, int gameId, int opponentId);
	EndgamePage* endgamePage;
};

