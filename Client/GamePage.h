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
	enum class PowerUpType {
		UNKNOWN = 0,
		SKIP_TURN,
		PLACE_ANY_CARD,
		DISCARD_CARD,
		NO_SCORE_LOSS,
		STEAL_CARD
	};

	GamePage(QWidget* parent = nullptr);
	GamePage(int playerId, const QString& playerName, int gameId, QWidget* parent = nullptr);
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

	void onPowerUpClicked(int powerUpId, PowerUpType powerUpType);
	void usePowerUp(int powerUpId, int targetPlayerId = INVALID_ID, int targetCardValue = INVALID_CARD_VALUE, int targetPileId = INVALID_PILE);
	void showStealCardDialog();
	void hideStealCardDialog();
	void enablePowerUps(bool enabled);

private:
	Ui::GameClass ui;

	

	int m_playerId = INVALID_ID;
	int m_gameId = INVALID_ID;
	int m_selectedCardValue = NO_SELECTED_CARD;
	int m_cardsPlacedThisTurn = 0;
	bool m_isMyTurn = false;
	QStringList m_allPlayerNames;

	
	QStringList m_chatLines;

	std::vector<int> m_lastHandCards;
	static constexpr int NUM_PILES = 4;
	static constexpr int INVALID_CARD_VALUE = -1;
	static constexpr int NO_SELECTED_CARD = -1;
	int m_lastPileValues[NUM_PILES] = { INVALID_CARD_VALUE, INVALID_CARD_VALUE, INVALID_CARD_VALUE, INVALID_CARD_VALUE };

	bool m_isFetchingState = false;

	static constexpr int INVALID_ID = -1;
	static constexpr int INVALID_PILE = -1;

	int m_activePowerUpId = INVALID_ID;
	PowerUpType m_activePowerUpType =PowerUpType::UNKNOWN;
	bool m_isPowerUpActive = false;

	int m_currentUnusedCards = 0;

	

	// UI for steal card
	QWidget* m_stealCardWidget = nullptr;
	QVBoxLayout* m_stealCardLayout = nullptr;

	GameTimer m_chatTimer;
	GameTimer m_stateTimer;

	QTimer* m_pollTimer;
	QNetworkAccessManager* net;

	QString m_playerName;

	void initializeGame(int playerId, const QString& playerName, int gameId);
	std::unique_ptr<EndgamePage> endgamePage;
};
