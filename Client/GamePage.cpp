#include "GamePage.h"

GamePage::GamePage(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);
	ui.AscendingDeck1Number->setStyleSheet(
		"background-image: url(:/Client/resources/1.png);"
		"background-repeat: no-repeat;"
		"background-position: center;"
	);
	ui.AscendingDeck2Number->setStyleSheet(
		"background-image: url(:/Client/resources/1.png);"
		"background-repeat: no-repeat;"
		"background-position: center;"
	);
	ui.DescendingDeck1Number->setStyleSheet(
		"background-image: url(:/Client/resources/100.png);"
		"background-repeat: no-repeat;"
		"background-position: center;"
	);
	ui.DescendingDeck2Number->setStyleSheet(
		"background-image: url(:/Client/resources/100.png);"
		"background-repeat: no-repeat;"
		"background-position: center;"
	);
}

GamePage::~GamePage()
{}

void GamePage::OnAscendingStack1Clicked()
{
	qDebug() << "Ascending Stack 1 clicked!";
}

void GamePage::OnAscendingStack2Clicked()
{
	qDebug() << "Ascending Stack 2 clicked!";
}

void GamePage::OnDescendingStack1Clicked()
{
	qDebug() << "Descending Stack 1 clicked!";
}

void GamePage::OnDescendingStack2Clicked()
{
	qDebug() << "Descending Stack 1 clicked!";
}

