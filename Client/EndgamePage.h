#pragma once

#include <QDialog>
#include "ui_EndgamePage.h"

class EndgamePage : public QDialog
{
	Q_OBJECT

public:
	EndgamePage(QWidget *parent = nullptr);
	~EndgamePage();

private:
	Ui::EndgamePageClass ui;
};

