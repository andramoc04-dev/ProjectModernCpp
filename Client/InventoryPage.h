#pragma once

#include <QDialog>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QEvent>
#include "ui_InventoryPage.h"
#include "../TimerDLL/GameTimer.h"
#include <unordered_map>

class InventoryPage : public QDialog
{
	Q_OBJECT

public:
	InventoryPage(QWidget* parent = nullptr);
	~InventoryPage();
	void setPlayerData(int playerId, const QString& playerName, int gameId);
	void fetchAvailablePowerUps();
	void showPowerUpOptions(std::unordered_set<QString> availablePowerups,
		const std::unordered_map<QString, int>& powerUpToInventoryId);
	void equipPowerUp(int powerUpId);
	void unequipPowerUp(int powerUpId);
	int getSelectedCount() const;
	void resetAllPowerUps();
protected:
	bool event(QEvent* e) override;

private:
	void updateTimerUI();
	void onTimerExpired();

	Ui::InventoryPageClass ui;
	QNetworkAccessManager* m_networkManager;
	int m_playerId;
	int m_gameId;
	QString m_playerName;

	GameTimer m_gameTimer;
	int m_totalSeconds;
	bool m_timerExpired = false;

	QList<QPushButton*> m_powerUpButtons;
	static const int MAX_EQUIPPED = 3;

};