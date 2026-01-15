#pragma once

#include <QDialog>
#include <QWidget>
#include <QPushButton>
#include <QGridLayout>
#include <QLabel>
#include <QEvent>

#include "ui_MinigamePage.h"
#include "../TimerDLL/GameTimer.h"

class MinigamePage : public QDialog
{
	Q_OBJECT

public:
	MinigamePage(QWidget* parent = nullptr);
	~MinigamePage();
	void RunMinigame();
	void submitMinigameScore(int playerId, int score, const QString& playerName, int gameId);
protected:
	bool event(QEvent* e) override;
	void closeEvent(QCloseEvent* event) override;
private:
	void updateTimerUi();

private:
	bool m_finishedEmitted = false;
	Ui::MinigameClass ui;
	int m_counter;

	GameTimer m_gameTimer;
	int m_totalSeconds = 10;
signals:
	void minigameFinished(int playerId, QString playerName, int gameId);

};

