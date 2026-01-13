#include "EndgamePage.h"


EndgamePage::EndgamePage(int playerId, QString gameEnding, const QString& serverUrl, QWidget* parent)
{
	ui.setupUi(this);
	if (gameEnding == "won") {
		this->setStyleSheet("border-image: url(:/Client/resources/win_background.jpeg) 0 0 0 0 stretch stretch;");
	}
	else {
		this->setStyleSheet("border-image: url(:/Client/resources/lose_background.jpeg) 0 0 0 0 stretch stretch;");
	}
	
}

EndgamePage::~EndgamePage()
{}

