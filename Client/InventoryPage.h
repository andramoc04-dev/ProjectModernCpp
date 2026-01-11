#pragma once

#include <QDialog>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QEvent>
#include "ui_InventoryPage.h"
#include "../TimerDLL/GameTimer.h"
#include <vector>


class InventoryPage : public QDialog
{
	Q_OBJECT

public:
	InventoryPage(QWidget* parent = nullptr);
	~InventoryPage();
	void setPlayerData(int playerId, const QString& playerName, int gameId, int opponentId);
	void fetchAvailablePowerUps();
	void showPowerUpOptions(std::vector<QString> availablePowerups);
protected:
	bool event(QEvent* e) override;

private:
	void updateTimerUI();
	void onTimerExpired();

	Ui::InventoryPageClass ui;
	QNetworkAccessManager* m_networkManager;
	int m_playerId;
	int m_gameId;
	int m_opponentId;
	QString m_playerName;

	GameTimer m_gameTimer;
	int m_totalSeconds;
	bool m_timerExpired = false;
signals:
	void inventoryFinished(int playerId, QString playerName, int gameId, int opponentId);
};