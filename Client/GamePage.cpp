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
	ShowCards(2);
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

void GamePage::ShowCards(uint16_t number)
{
	QString style =
		"QPushButton {"
		"    border: none;"
		"    background-image: url(:/Client/resources/Card.png);"
		"}";
	ui.layout->setSpacing(0);
	ui.layout->setContentsMargins(0, 0, 0, 0);
	for (int i = 0; i < 8; i++) {
		QPushButton* button = new QPushButton(this);
		button->setStyleSheet(style);
		button->setFixedSize(121, 492);

		int randomCard = std::rand() % 99+1;

		if(randomCard>=10)
		{
			QLabel* nr1 = new QLabel("", button);
			nr1->setFixedSize(32, 41);
			nr1->move(28, 90);
			QString imgPath1 = QString(":/Client/resources/%1.png").arg(randomCard/10);
			qDebug() << "Image path 1:" << imgPath1;
			nr1->setStyleSheet(
				QString("QLabel {"
					"    background-image: url(%1);"
					"    background-repeat: no-repeat;"
					"    background-position: center;"
					"}").arg(imgPath1)
			);
			QLabel* nr2 = new QLabel("", button);
			nr2->setFixedSize(32, 41);
			nr2->move(56, 90);
			QString imgPath2 = QString(":/Client/resources/%1.png").arg(randomCard % 10);
			qDebug() << "Image path 2:" << imgPath2;
			nr2->setStyleSheet(
				QString("QLabel {"
					"    background-image: url(%1);"
					"    background-repeat: no-repeat;"
					"    background-position: center;"
					"}").arg(imgPath2)
			);
		}
		else
		{
			QLabel* nr = new QLabel("", button);
			nr->setFixedSize(32, 41);
			nr->move(45, 90);
			QString imgPath1 = QString(":/Client/resources/%1.png").arg(randomCard);
			qDebug() << "Image path 1:" << imgPath1;
			nr->setStyleSheet(
				QString("QLabel {"
					"    background-image: url(%1);"
					"    background-repeat: no-repeat;"
					"    background-position: center;"
					"}").arg(imgPath1)
			);
		}
		ui.layout->addWidget(button, 0, i);

	}

}


