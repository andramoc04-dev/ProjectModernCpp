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
#include <QMessageBox>
#include <QPainter>

GamePage::GamePage(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);
	net = new QNetworkAccessManager(this);

	connect(ui.sendButton, &QPushButton::clicked,
		this, &GamePage::onSendClicked);

	// pornește timerele DLL
	m_chatTimer.start(1);   // 1 sec
	m_stateTimer.start(2);  // 2 sec

	m_pollTimer = new QTimer(this);
	connect(m_pollTimer, &QTimer::timeout, this, &GamePage::checkTimers);
	m_pollTimer->start(50);

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

	updatePileVisual(ui.AscendingDeck1Number, 1);
	updatePileVisual(ui.AscendingDeck2Number, 1);
	updatePileVisual(ui.DescendingDeck1Number, 100);
	updatePileVisual(ui.DescendingDeck2Number, 100);

}

GamePage::GamePage(int playerId,const QString& playerName, int gameId, int opponentId, QWidget* parent) : GamePage(parent)
{
	initializeGame(playerId,playerName, gameId, opponentId);
	shuffleDeck();
}

GamePage::~GamePage()
{
	// Oprește QTimer
	if (m_pollTimer) {
		m_pollTimer->stop();
	}

	// Oprește GameTimer DLL
	m_chatTimer.stop();
	m_stateTimer.stop();
}


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
	
	QLayoutItem* item;
	while ((item = ui.layout->takeAt(0)) != nullptr) {
		if (item->widget()) {
			delete item->widget();
		}
		delete item;
	}


	QString style =
		"QPushButton {"
		"    border: none;"
		"    background-image: url(:/Client/resources/CardButton.png);"
		"}";

	ui.layout->setSpacing(0);
	ui.layout->setContentsMargins(0, 0, 0, 0);

	int index = 0;
	for (const auto& cardValue : cardsValues) {
		QPushButton* button = new QPushButton(this);
		button->setStyleSheet(style);
		button->setFixedSize(100, 315);

		// --- partea grafică pentru număr ---
		if (cardValue >= 10) {
			QLabel* nr1 = new QLabel("", button);
			nr1->setFixedSize(32, 41);
			nr1->move(22, 45);
			QString imgPath1 = QString(":/Client/resources/%1.png").arg(cardValue / 10);
			nr1->setStyleSheet(QString("QLabel { background-image: url(%1); }").arg(imgPath1));

			QLabel* nr2 = new QLabel("", button);
			nr2->setFixedSize(32, 41);
			nr2->move(50, 45);
			QString imgPath2 = QString(":/Client/resources/%1.png").arg(cardValue % 10);
			nr2->setStyleSheet(QString("QLabel { background-image: url(%1); }").arg(imgPath2));
		}
		else {
			QLabel* nr = new QLabel("", button);
			nr->setFixedSize(32, 41);
			nr->move(32, 45);
			QString imgPath1 = QString(":/Client/resources/%1.png").arg(cardValue);
			nr->setStyleSheet(QString("QLabel { background-image: url(%1); }").arg(imgPath1));
		}

		// --- nou: click pe carte setează valoarea selectată ---
		connect(button, &QPushButton::clicked, this, [=]() {
			m_selectedCardValue = cardValue;
			qDebug() << "Selected card:" << m_selectedCardValue;
			});

		ui.layout->addWidget(button, 0, index++);
	}



}

void GamePage::updatePileVisual(QLabel* pileLabel, int cardValue)
{
	qDebug() << "updatePileVisual called with value:" << cardValue;

	QPixmap pix;

	if (cardValue == 100) {
		pix.load(":/Client/resources/100.png");
	}
	else if (cardValue >= 10) {
		// compunem imaginea din două cifre
		QPixmap left(QString(":/Client/resources/%1.png").arg(cardValue / 10));
		QPixmap right(QString(":/Client/resources/%1.png").arg(cardValue % 10));

		// lipim cele două imagini într-un pixmap nou
		pix = QPixmap(left.width() + right.width(), left.height());
		pix.fill(Qt::transparent);

		QPainter painter(&pix);
		painter.drawPixmap(0, 0, left);
		painter.drawPixmap(left.width(), 0, right);
	}
	else {
		pix.load(QString(":/Client/resources/%1.png").arg(cardValue));
	}

	if (!pix.isNull()) {
		pileLabel->setPixmap(pix.scaled(pileLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
		pileLabel->update();
	}
	else {
		qDebug() << "Eroare: nu găsesc resursa pentru valoarea" << cardValue;
	}


}

void GamePage::fetchGameState()
{
	QNetworkRequest req(QUrl(QString("http://127.0.0.1:18080/game/%1/state").arg(m_gameId)));
	QNetworkReply* reply = net->get(req);

	connect(reply, &QNetworkReply::finished, this, [=]() {
		auto data = reply->readAll();
		QJsonDocument doc = QJsonDocument::fromJson(data);

		if (!doc.isObject()) {
			qDebug() << "Invalid JSON received";
			reply->deleteLater();
			return;
		}

		QJsonObject obj = doc.object();

		int currentPlayerId = obj["currentPlayerId"].toInt();
		QString currentPlayerName = obj["currentPlayerName"].toString();
		bool isMyTurn = (currentPlayerId == m_playerId);

		ui.DescendingCardPlacer1->setEnabled(isMyTurn);
		ui.DescendingCardPlacer2->setEnabled(isMyTurn);
		ui.AscendingCardPlacer1->setEnabled(isMyTurn);
		ui.AscendingCardPlacer2->setEnabled(isMyTurn);
		ui.deckButton->setEnabled(isMyTurn);

		if (isMyTurn) {
			this->setWindowTitle(QString("YOUR TURN | Team: %1").arg(m_allPlayerNames.join(", ")));
		}
		else {
			this->setWindowTitle(QString("%1's turn | Team: %2")
				.arg(currentPlayerName)
				.arg(m_allPlayerNames.join(", ")));
		}

		QJsonArray piles = obj["piles"].toArray();
		updatePileVisual(ui.DescendingDeck1Number, piles[0].toInt());
		updatePileVisual(ui.DescendingDeck2Number, piles[1].toInt());
		updatePileVisual(ui.AscendingDeck1Number, piles[2].toInt());
		updatePileVisual(ui.AscendingDeck2Number, piles[3].toInt());

		QJsonArray players = obj["players"].toArray();
		m_allPlayerNames.clear();
		for (const auto& playerVal : players) {
			QJsonObject playerObj = playerVal.toObject();
			QString username = playerObj["username"].toString();
			m_allPlayerNames << username;
		}

		reply->deleteLater();
		});
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
	m_cardsPlacedThisTurn = 0;
	

	

	net = new QNetworkAccessManager(this);

	connect(ui.DescendingCardPlacer1, &QPushButton::clicked, this, [=]() {
		onPileClicked(0, ui.DescendingDeck1Number);
		});
	connect(ui.DescendingCardPlacer2, &QPushButton::clicked, this, [=]() {
		onPileClicked(1, ui.DescendingDeck2Number);
		});
	connect(ui.AscendingCardPlacer1, &QPushButton::clicked, this, [=]() {
		onPileClicked(2, ui.AscendingDeck1Number);
		});
	connect(ui.AscendingCardPlacer2, &QPushButton::clicked, this, [=]() {
		onPileClicked(3, ui.AscendingDeck2Number);
		});

	connect(ui.deckButton, &QPushButton::clicked, this, &GamePage::onDrawCardClicked);

	updatePileVisual(ui.AscendingDeck1Number, 1);
	updatePileVisual(ui.AscendingDeck2Number, 1);
	updatePileVisual(ui.DescendingDeck1Number, 100);
	updatePileVisual(ui.DescendingDeck2Number, 100);

	fetchGameState();

}

void GamePage::onSendClicked()
{
	QString text = ui.chatInput->text().trimmed();
	if (text.isEmpty()) return;

	QJsonObject obj;
	obj["playerName"] = m_playerName;
	obj["text"] = ui.chatInput->text();

	
	QNetworkRequest req(QUrl(
		QString("http://127.0.0.1:18080/game/%1/chat/send").arg(m_gameId)
	));

	req.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

	net->post(req, QJsonDocument(obj).toJson());
	ui.chatInput->clear();
}


void GamePage::onPileClicked(int pileId, QLabel* pileLabel)
{
	if (m_selectedCardValue == -1) {
		QMessageBox::warning(this, "No card selected", "Select a card before choosing a card placer.");
		return;
	}

	QJsonObject obj;
	obj["cardValue"] = m_selectedCardValue;
	obj["pileId"] = pileId;

	QNetworkRequest req(QUrl(QString("http://localhost:18080/game/%1/player/%2/place")
		.arg(m_gameId).arg(m_playerId)));
	req.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

	QNetworkReply* reply = net->post(req, QJsonDocument(obj).toJson());
	connect(reply, &QNetworkReply::finished, this, [=]() {
		auto data = reply->readAll();
		QJsonDocument doc = QJsonDocument::fromJson(data);
		QJsonObject respObj = doc.object();

		qDebug() << "Server response:" << doc;

		if (doc["success"].toBool()) {
			int newPileValue = doc["newPileValue"].toInt();
			int cardsPlaced = respObj["cardsPlacedThisTurn"].toInt();

			updatePileVisual(pileLabel, newPileValue);
			m_selectedCardValue = -1;
			m_cardsPlacedThisTurn = cardsPlaced;
			FetchHand(); // reîncarcă mâna
		}
		else {
			QMessageBox::warning(this, "Invalid move", "You can't place this card on the chosen card placer.");
		}
		reply->deleteLater();
		});


}

void GamePage::onDrawCardClicked()
{
	QNetworkRequest endTurnReq(QUrl(QString("http://127.0.0.1:18080/game/%1/player/%2/endturn")
		.arg(m_gameId).arg(m_playerId)));
	endTurnReq.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

	QNetworkReply* reply = net->post(endTurnReq, QByteArray());

	connect(reply, &QNetworkReply::finished, [this, reply]() {
		auto data = reply->readAll();
		QJsonDocument doc = QJsonDocument::fromJson(data);
		QJsonObject obj = doc.object();

		if (reply->error() == QNetworkReply::NoError && obj["success"].toBool()) {
			int cardsPlaced = obj["cardsPlaced"].toInt();
			int cardsDrawn = obj["cardsDrawn"].toInt();
			int cardsLeftInDeck = obj["cardsLeftInDeck"].toInt();
			QString message = obj["message"].toString();

			qDebug() << "Turn ended. Cards placed:" << cardsPlaced
				<< "Cards drawn:" << cardsDrawn;

			QMessageBox::information(this, "Turn Ended", message);
			m_cardsPlacedThisTurn = 0;
			FetchHand();
		}
		else {
			// Eroare - probabil nu a plasat destule cărți
			QString errorMsg = "Unknown error";
			if (obj.contains("error")) {
				errorMsg = obj["error"].toString();
			}

			qDebug() << "Error ending turn:" << errorMsg;
			QMessageBox::warning(this, "Cannot End Turn", errorMsg);
		}
		reply->deleteLater();
		});
}

void GamePage::checkTimers()
{	// Verifică dacă timer-ul de chat a expirat
	if (m_chatTimer.expired()) {
		fetchMessages();
		m_chatTimer.start(1);  // Restart pentru următorul ciclu
	}

	// Verifică dacă timer-ul de game state a expirat
	if (m_stateTimer.expired()) {
		fetchGameState();
		m_stateTimer.start(2);  // Restart pentru următorul ciclu
	}
}



void GamePage::fetchMessages()
{
	
	QNetworkRequest req(QUrl(
		QString("http://127.0.0.1:18080/game/%1/chat/messages").arg(m_gameId)
	));

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

