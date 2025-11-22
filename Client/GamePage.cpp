#include "GamePage.h"

GamePage::GamePage(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);
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

