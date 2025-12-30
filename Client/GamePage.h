#pragma once

#include <QDialog>
#include <QGridLayout>
#include <QNetworkAccessManager>
#include "../TimerDLL/GameTimer.h"
#include "ui_GamePage.h"
#include <thread>
#include <atomic>





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
	
private slots:
	void onSendClicked();
	void fetchMessages();
	void onPileClicked(int pileId, QLabel* pileLabel);
	void onDrawCardClicked();


private:
	Ui::GameClass ui;
	int m_playerId = -1;
	int m_gameId = -1;
	int m_opponentId = -1;
	int m_selectedCardValue = -1;
	int m_cardsPlacedThisTurn = 0;
	QStringList m_allPlayerNames;

	std::atomic_bool m_pollRunning{ false };
	std::thread m_pollThread;

	GameTimer m_chatTimer;
	GameTimer m_stateTimer;


	QNetworkAccessManager* net;
	

	QString m_playerName; // setat după login

	void initializeGame(int playerId,const QString& playerName, int gameId, int opponentId);
};

