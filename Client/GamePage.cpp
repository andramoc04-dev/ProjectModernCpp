#include "GamePage.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QNetworkReply>
#include <QDateTime>
#include <QScrollBar>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QDebug>
#include <cstdlib>

GamePage::GamePage(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);
	net = new QNetworkAccessManager(this);

	connect(ui.sendButton, &QPushButton::clicked, this, &GamePage::onSendClicked);

	chatTimer = new QTimer(this);
	connect(chatTimer, &QTimer::timeout, this, &GamePage::fetchMessages);
	chatTimer->start(800);

	ui.chatInput->setStyleSheet(
		"   background: transparent;"
		"   border: none;"
		"   color: #573C27;"
		"   font-family: 'Showcard Gothic';"
		"   font-weight: bold;"
		"   font-size: 14pt;"
	);

	ui.chatMessages->setStyleSheet(
		"   background: transparent;"
		"   border: none;"
		"   color: #573C27;"
		"   font-family: 'Showcard Gothic';"
		"   font-weight: bold;"
		"   font-size: 14pt;"
	);


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

GamePage::GamePage(int playerId, int gameId, int opponentId, QWidget* parent) : GamePage(parent)
{
	initializeGame(playerId, gameId, opponentId);
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
		"    background-image: url(:/Client/resources/CardButton.png);"
		"}";
	ui.layout->setSpacing(0);
	ui.layout->setContentsMargins(0, 0, 0, 0);
	for (int i = 0; i < 8; i++) {
		QPushButton* button = new QPushButton(this);
		button->setStyleSheet(style);
		button->setFixedSize(100, 315);

		int randomCard = std::rand() % 99+1;

		if(randomCard>=10)
		{
			QLabel* nr1 = new QLabel("", button);
			nr1->setFixedSize(32, 41);
			nr1->move(22, 45);
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
			nr2->move(50, 45);
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
			nr->move(32, 45);
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

void GamePage::initializeGame(int playerId, int gameId, int opponentId)
{
	m_playerId = playerId;
	m_gameId = gameId;
	m_opponentId = opponentId;

	// Setăm un titlu util pentru fereastră
	this->setWindowTitle(QString("Game %1 — You: %2 vs %3").arg(m_gameId).arg(m_playerId).arg(m_opponentId));
}

void GamePage::onSendClicked()
{
	QString text = ui.chatInput->text().trimmed();
	if (text.isEmpty()) return;

	QJsonObject obj;
	obj["playerName"] = playerName;
	obj["text"] = text;

	QNetworkRequest req(QUrl("http://127.0.0.1:18080/chat/send"));
	req.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

	net->post(req, QJsonDocument(obj).toJson());
	ui.chatInput->clear();
}

void GamePage::fetchMessages()
{
	QNetworkRequest req(QUrl("http://127.0.0.1:18080/chat/messages"));
	QNetworkReply* reply = net->get(req);

	connect(reply, &QNetworkReply::finished, [this, reply]()
		{
			if (reply->error() == QNetworkReply::NoError)
			{
				QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());
				if (doc.isArray())
				{
					QString all;
					for (auto v : doc.array())
					{
						auto o = v.toObject();
						all += QString("%1: %2\n")
							.arg(o["playerName"].toString())
							.arg(o["text"].toString());
					}
					ui.chatMessages->setPlainText(all);
				}
			}
			reply->deleteLater();
		});
}

