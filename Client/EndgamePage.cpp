#include "EndgamePage.h"


EndgamePage::EndgamePage(int playerId, QString gameEnding, const QString& serverUrl, QWidget* parent)
{
	ui.setupUi(this);
	gameEnding == "won" ?
		this->setStyleSheet("background-image: url(:/images/win_background.jpg);"):
		this->setStyleSheet("background-image: url(:/images/lose_background.jpg);");
	
}

EndgamePage::~EndgamePage()
{}

