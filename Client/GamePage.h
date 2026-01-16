#pragma once

#include <QDialog>
#include <QGridLayout>
#include <QNetworkAccessManager>
#include <QTimer> 
#include "../TimerDLL/GameTimer.h"
#include "ui_GamePage.h"
#include "EndgamePage.h"





class GamePage : public QDialog
{
	Q_OBJECT

public:
	GamePage(QWidget *parent = nullptr);
	GamePage(int playerId,const QString& playerName, int gameId, QWidget* parent = nullptr);
	~GamePage();

	void FetchHand();
	void ShowCards(const std::vector<int>& cardsValues);
	void shuffleDeck();
	void DistributeCards();
	void updatePileVisual(QLabel* pileLabel, int cardValue);
	void fetchGameState();
	void showPowerUps(const std::unordered_set<int>& equippedPowerUpIds);
	void fetchEquippedPowerUps();
private slots:
	void onSendClicked();
	void fetchMessages();
	void onPileClicked(int pileId, QLabel* pileLabel);
	void onDrawCardClicked();
	void checkTimers();
	void openEndgamePage(QString gameStatus);

	void onPowerUpClicked(int powerUpId, int powerUpType);
	void usePowerUp(int powerUpId, int targetPlayerId = -1, int targetCardValue = -1, int targetPileId = -1);
	void showStealCardDialog();
	void hideStealCardDialog();
	void enablePowerUps(bool enabled);

private:
	Ui::GameClass ui;
	int m_playerId = -1;
	int m_gameId = -1;
	int m_selectedCardValue = -1;
	int m_cardsPlacedThisTurn = 0;
	bool m_isMyTurn = false;
	QStringList m_allPlayerNames;
	
	static constexpr int MAX_CHAT_MESSAGES = 8;
	QStringList m_chatLines;


	std::vector<int> m_lastHandCards;      
	int m_lastPileValues[4] = { -1, -1, -1, -1 }; 
	bool m_isFetchingState = false;

	int m_activePowerUpId = -1;
	int m_activePowerUpType = -1;
	bool m_isPowerUpActive = false;

	int m_currentUnusedCards = 0;

	// Power-up type constants
	static const int POWERUP_SKIP_TURN = 1;
	static const int POWERUP_PLACE_ANY_CARD = 2;
	static const int POWERUP_DISCARD_CARD = 3;
	static const int POWERUP_NO_SCORE_LOSS = 4;
	static const int POWERUP_STEAL_CARD = 5;

	// UI for steal card
	QWidget* m_stealCardWidget = nullptr;
	QVBoxLayout* m_stealCardLayout = nullptr;


	GameTimer m_chatTimer;
	GameTimer m_stateTimer;

	QTimer* m_pollTimer;

	QNetworkAccessManager* net;
	

	QString m_playerName; 

	void initializeGame(int playerId,const QString& playerName, int gameId);
	std::unique_ptr<EndgamePage> endgamePage;
};

