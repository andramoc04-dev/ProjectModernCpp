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
#include <QNetworkRequest> 
#include <QNetworkAccessManager> 

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
}

GamePage::GamePage(int playerId,const QString& playerName, int gameId, int opponentId, QWidget* parent) : GamePage(parent)
{
	initializeGame(playerId,playerName, gameId, opponentId);
	shuffleDeck();
}

GamePage::~GamePage()
{}


void GamePage::FetchHand()
{

	QString urlString = QString("http://127.0.0.1:18080/game/%1/player/%2/hand")
		.arg(m_gameId)
		.arg(m_playerId);
	QUrl url(urlString);

	QNetworkRequest request;
	request.setUrl(url);
	QNetworkReply* reply = net->get(request);

	connect(reply, &QNetworkReply::finished, [this, reply]() {
		if (reply->error() == QNetworkReply::NoError) {
			QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());
			QJsonObject obj = doc.object();

			qDebug() << "Hand received for player" << obj["playerId"].toInt();
			qDebug() << "Game ID:" << obj["gameId"].toInt();

			QJsonArray handArray = obj["hand"].toArray();
			qDebug() << "Cards in hand:";
			std::vector<int> cardsValues;
			for (const QJsonValue& cardValue : handArray) {
				QJsonObject card = cardValue.toObject();
				qDebug() << "  Card value:" << card["value"].toInt();
				cardsValues.push_back(card["value"].toInt());
			}
			 
			ShowCards(cardsValues);
		}
		else {
			qDebug() << "Error fetching hand:" << reply->errorString();
		}
		reply->deleteLater();
		});
}

void GamePage::ShowCards(const std::vector<int>& cardsValues)
{
	QString style =
		"QPushButton {"
		"    border: none;"
		"    background-image: url(:/Client/resources/CardButton.png);"
		"}";
	ui.layout->setSpacing(0);
	ui.layout->setContentsMargins(0, 0, 0, 0);
	int index = 0;
	for (const auto& cardValue: cardsValues) {
		QPushButton* button = new QPushButton(this);
		button->setStyleSheet(style);
		button->setFixedSize(100, 315);

		if(cardValue >=10)
		{
			QLabel* nr1 = new QLabel("", button);
			nr1->setFixedSize(32, 41);
			nr1->move(22, 45);
			QString imgPath1 = QString(":/Client/resources/%1.png").arg(cardValue /10);
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
			QString imgPath2 = QString(":/Client/resources/%1.png").arg(cardValue % 10);
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
			QString imgPath1 = QString(":/Client/resources/%1.png").arg(cardValue);
			qDebug() << "Image path 1:" << imgPath1;
			nr->setStyleSheet(
				QString("QLabel {"
					"    background-image: url(%1);"
					"    background-repeat: no-repeat;"
					"    background-position: center;"
					"}").arg(imgPath1)
			);
		}
		ui.layout->addWidget(button, 0, index++);
	}

}

void GamePage::shuffleDeck()
{
	QJsonObject obj;
	obj["gameId"] = m_gameId;

	QString urlString = QString("http://127.0.0.1:18080/game/%1/shuffle").arg(m_gameId);
	QUrl url(urlString);

	QNetworkRequest request;
	request.setUrl(url);
	request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

	QByteArray jsonData = QJsonDocument(obj).toJson();
	QNetworkReply* reply = net->post(request, jsonData);

	connect(reply, &QNetworkReply::finished, [this, reply]() {
		if (reply->error() == QNetworkReply::NoError) {
			QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());
			QJsonObject obj = doc.object();

			qDebug() << "Deck shuffled successfully for game" << obj["gameId"].toInt();
			qDebug() << "Cards in deck:" << obj["cardsInDeck"].toInt();
		}
		else {
			qDebug() << "Error shuffling deck:" << reply->errorString();
		}
		DistributeCards();
		reply->deleteLater();
		});
	
}

void GamePage::DistributeCards()
{
	QJsonObject obj;
	obj["gameId"] = m_gameId;

	QString urlString = QString("http://127.0.0.1:18080/game/%1/distribution").arg(m_gameId);
	QUrl url(urlString);

	QNetworkRequest request;
	request.setUrl(url);
	request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

	QByteArray jsonData = QJsonDocument(obj).toJson();
	QNetworkReply* reply = net->post(request, jsonData);

	connect(reply, &QNetworkReply::finished, [this, reply]() {
		if (reply->error() == QNetworkReply::NoError) {
			QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());
			QJsonObject obj = doc.object();

			qDebug() << "Cards distributed successfully for game" << obj["gameId"].toInt();
			qDebug() << "Current status:" << obj["status"].toInt();
		}
		else {
			qDebug() << "Error distributing cards:" << reply->errorString();
		}
		FetchHand();
		reply->deleteLater();
		});
}

void GamePage::initializeGame(int playerId,const QString& playerName, int gameId, int opponentId)
{
	m_playerId = playerId;
	m_playerName = playerName;
	m_gameId = gameId;
	m_opponentId = opponentId;
	
	this->setWindowTitle(QString("Game %1 — You: %2 vs %3").arg(m_gameId).arg(m_playerId).arg(m_opponentId));
}

void GamePage::onSendClicked()
{
	QString text = ui.chatInput->text().trimmed();
	if (text.isEmpty()) return;

	QJsonObject obj;
	obj["playerName"] = m_playerName;
	obj["text"] = ui.chatInput->text();

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
					for (const auto& v : doc.array())
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

