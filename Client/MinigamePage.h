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
	MinigamePage(QWidget *parent = nullptr);
	~MinigamePage();
	void RunMinigame();

protected:
	bool event(QEvent* e) override;

private:
	void updateTimerUi();

private:
	Ui::MinigameClass ui;
	int m_counter;

	GameTimer m_gameTimer;
	int m_totalSeconds = 30;
};

