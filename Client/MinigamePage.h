#pragma once

#include <QDialog>
#include <QWidget>
#include <QPushButton>
#include <QGridLayout>
#include "ui_MinigamePage.h"

class MinigamePage : public QDialog
{
	Q_OBJECT

public:
	MinigamePage(QWidget *parent = nullptr);
	~MinigamePage();
	void RunMinigame();

private:
	Ui::MinigameClass ui;
	int m_counter;
};

