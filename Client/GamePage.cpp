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
	m_stateTimer.start(2);  // 1 sec

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

	// Cleanup steal card widget
	if (m_stealCardWidget) {
		delete m_stealCardWidget;
		m_stealCardWidget = nullptr;
	}
}
void GamePage::fetchEquippedPowerUps()
{
	QString urlString = QString("http://127.0.0.1:18080/player/%1/powerups/equipped")
		.arg(m_playerId);
	QUrl url(urlString);

	QNetworkRequest request;
	request.setUrl(url);
	QNetworkReply* reply = net->get(request);

	connect(reply, &QNetworkReply::finished, [this, reply]() {
		if (reply->error() == QNetworkReply::NoError) {
			QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());
			QJsonObject obj = doc.object();

			qDebug() << "Equipped power-ups received for player" << obj["playerId"].toInt();
			qDebug() << "Total equipped:" << obj["totalEquipped"].toInt();

			QJsonArray powerUpsArray = obj["powerUps"].toArray();
			qDebug() << "Equipped power-ups count:" << powerUpsArray.size();

			std::vector<int> equippedPowerUpIds;
			for (const QJsonValue& powerUpValue : powerUpsArray) {
				QJsonObject powerUp = powerUpValue.toObject();
				int powerUpId = powerUp["id"].toInt();
				QString type = powerUp["type"].toString();
				QString name = powerUp["name"].toString();

				qDebug() << "  Equipped Power-up ID:" << powerUpId
					<< "Type:" << type
					<< "Name:" << name;

				equippedPowerUpIds.push_back(powerUpId);
			}

			showPowerUps(equippedPowerUpIds);
		}
		else {
			qDebug() << "Error fetching equipped power-ups:" << reply->errorString();
		}
		reply->deleteLater();
		});
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
			fetchGameState();
		}
		else {
			qDebug() << "Error fetching hand:" << reply->errorString();
		}
		reply->deleteLater();
		});

}

void GamePage::ShowCards(const std::vector<int>& cardsValues)
{
	if (m_lastHandCards == cardsValues) {
		return;
	}
	m_lastHandCards = cardsValues;

	qDebug() << "Redrawing hand (change detected)...";

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

		connect(button, &QPushButton::clicked, this, [=]() {
			if (m_isPowerUpActive) {
				if (m_activePowerUpType == POWERUP_DISCARD_CARD) {
					usePowerUp(m_activePowerUpId, -1, cardValue, -1);
					m_isPowerUpActive = false;
					m_activePowerUpId = -1;
					m_activePowerUpType = -1;
				}
				else if (m_activePowerUpType == POWERUP_PLACE_ANY_CARD) {
					m_selectedCardValue = cardValue;
					QMessageBox::information(this, "Wild Card",
						QString("Card %1 selected. Now choose any pile.").arg(cardValue));
				}
				else {
					m_selectedCardValue = cardValue;
					qDebug() << "Selected card:" << m_selectedCardValue;
				}
			}
			else {
				m_selectedCardValue = cardValue;
				qDebug() << "Selected card:" << m_selectedCardValue;
			}
			});

		ui.layout->addWidget(button, 0, index++);
	}
}

void GamePage::updatePileVisual(QLabel* pileLabel, int cardValue)
{
	int pileIndex = -1;
	if (pileLabel == ui.DescendingDeck1Number) pileIndex = 0;
	else if (pileLabel == ui.DescendingDeck2Number) pileIndex = 1;
	else if (pileLabel == ui.AscendingDeck1Number) pileIndex = 2;
	else if (pileLabel == ui.AscendingDeck2Number) pileIndex = 3;

	if (pileIndex != -1 && m_lastPileValues[pileIndex] == cardValue) {
		return;
	}

	if (pileIndex != -1) {
		m_lastPileValues[pileIndex] = cardValue;
	}

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


void GamePage::enablePowerUps(bool enabled)
{
	// Parcurge toate widget-urile din powerupsGrid și activează/dezactivează butoanele
	for (int i = 0; i < ui.powerupsGrid->count(); ++i) {
		QLayoutItem* item = ui.powerupsGrid->itemAt(i);
		if (item && item->widget()) {
			QPushButton* button = qobject_cast<QPushButton*>(item->widget());
			if (button) {
				button->setEnabled(enabled);

				// Schimbă stilul vizual pentru a indica că sunt disabled
				if (!enabled) {
					button->setStyleSheet(button->styleSheet() +
						"QPushButton:disabled {"
						"    opacity: 0.5;"
						"    background-color: #cccccc;"
						"}");
				}
			}
		}
	}
}

void GamePage::fetchGameState()
{
	if (m_isFetchingState) return;

	m_isFetchingState = true;

	QNetworkRequest req(QUrl(QString("http://127.0.0.1:18080/game/%1/state").arg(m_gameId)));
	QNetworkReply* reply = net->get(req);

	connect(reply, &QNetworkReply::finished, this, [=]() {
		m_isFetchingState = false; 

		if (reply->error() != QNetworkReply::NoError) {
			reply->deleteLater();
			return;
		}

		auto data = reply->readAll();
		QJsonDocument doc = QJsonDocument::fromJson(data);

		if (!doc.isObject()) {
			reply->deleteLater();
			return;
		}

		QJsonObject obj = doc.object();

		QString gameStatus = obj["gameStatus"].toString();
		if (gameStatus == "won" || gameStatus == "lost") {
			m_pollTimer->stop();
			m_chatTimer.stop();
			m_stateTimer.stop();
			openEndgamePage(gameStatus);
			reply->deleteLater();
			return;
		}

		int currentPlayerId = obj["currentPlayerId"].toInt();
		QString currentPlayerName = obj["currentPlayerName"].toString();
		m_isMyTurn = (currentPlayerId == m_playerId);

		// Activează/Dezactivează butoanele de plasare
		// (Nu mai e nevoie să verifici isEnabled anterior, setEnabled e destul de eficient în Qt)
		ui.DescendingCardPlacer1->setEnabled(m_isMyTurn);
		ui.DescendingCardPlacer2->setEnabled(m_isMyTurn);
		ui.AscendingCardPlacer1->setEnabled(m_isMyTurn);
		ui.AscendingCardPlacer2->setEnabled(m_isMyTurn);
		ui.deckButton->setEnabled(m_isMyTurn);

		// Apelăm enablePowerUps pentru a actualiza butoanele existente
		enablePowerUps(m_isMyTurn);

		QJsonArray players = obj["players"].toArray();
		m_allPlayerNames.clear(); // Golim lista veche
		for (const auto& playerVal : players) {
			QJsonObject playerObj = playerVal.toObject();
			QString username = playerObj["username"].toString();
			m_allPlayerNames << username; // Adăugăm numele în listă
		}
		QString newTitle;
		if (m_isMyTurn) newTitle = QString("Your turn | Team: %1").arg(m_allPlayerNames.join(", "));
		else newTitle = QString("%1's turn | Team: %2").arg(currentPlayerName).arg(m_allPlayerNames.join(", "));

		if (this->windowTitle() != newTitle) {
			this->setWindowTitle(newTitle);
		}

		QJsonArray piles = obj["piles"].toArray();
		updatePileVisual(ui.DescendingDeck1Number, piles[0].toInt());
		updatePileVisual(ui.DescendingDeck2Number, piles[1].toInt());
		updatePileVisual(ui.AscendingDeck1Number, piles[2].toInt());
		updatePileVisual(ui.AscendingDeck2Number, piles[3].toInt());

		FetchHand();

		reply->deleteLater();
		});
}

void GamePage::showPowerUps(const std::vector<int>& equippedPowerUpIds)
{
	QLayoutItem* item;
	while ((item = ui.powerupsGrid->takeAt(0)) != nullptr) {
		if (QWidget* w = item->widget()) {
			w->deleteLater();
		}
		delete item;
	}

	ui.powerupsGrid->setSpacing(10);
	ui.powerupsGrid->setContentsMargins(10, 10, 10, 10);

	if (equippedPowerUpIds.empty()) {
		qDebug() << "No equipped power-ups to display";
		return;
	}

	int row = 0;
	for (const auto& powerUpId : equippedPowerUpIds) {
		QPushButton* button = new QPushButton(this);
		button->setFixedSize(100, 100);

		QString imgPath = QString(":/Client/resources/powerup%1.jpg").arg(powerUpId);

		qDebug() << "Creating power-up button for ID:" << powerUpId << "at row:" << row;

		button->setStyleSheet(QString(
			"QPushButton {"
			"    border: 2px solid #333;"
			"    border-radius: 5px;"
			"    background: #4CAF50 url(%1) no-repeat center;"
			"    background-size: cover;"
			"}"
			"QPushButton:hover {"
			"    border-color: #FFD700;"
			"    background-color: #45a049;"
			"}"
			"QPushButton:pressed {"
			"    background-color: #3d8b40;"
			"}"
			"QPushButton:disabled {"
			"    opacity: 0.5;"
			"    background-color: #cccccc;"
			"    border-color: #999;"
			"}"
		).arg(imgPath));

		button->setProperty("powerUpId", powerUpId);

		int powerUpType = powerUpId; // ID-ul este același cu type-ul (1-5)

		connect(button, &QPushButton::clicked, [this, powerUpId, powerUpType]() {
			onPowerUpClicked(powerUpId, powerUpType);
			});

		// Inițial disabled până verificăm dacă este tura jucătorului
		button->setEnabled(m_isMyTurn);

		ui.powerupsGrid->addWidget(button, row, Qt::AlignCenter);
		row++;
	}

	qDebug() << "Displayed" << equippedPowerUpIds.size() << "equipped power-ups vertically";
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
	

	m_isMyTurn = false;

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

	//fetchGameState();
	fetchEquippedPowerUps();
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

	// Verifică dacă Wild Card este activ
	if (m_isPowerUpActive && m_activePowerUpType == POWERUP_PLACE_ANY_CARD) {
		usePowerUp(m_activePowerUpId, -1, m_selectedCardValue, pileId);
		m_isPowerUpActive = false;
		m_activePowerUpId = -1;
		m_activePowerUpType = -1;
		m_selectedCardValue = -1;
		return;
	}

	// Comportament normal
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
			FetchHand();
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

void GamePage::openEndgamePage(QString gameStatus)
{

	endgamePage = new EndgamePage(m_playerId, gameStatus,"http://127.0.0.1:18080", nullptr);
	endgamePage->show();
	this->close();
}

void GamePage::onPowerUpClicked(int powerUpId, int powerUpType)
{
	qDebug() << "Power-up clicked:" << powerUpId << "Type:" << powerUpType;

	m_activePowerUpId = powerUpId;
	m_activePowerUpType = powerUpType;
	m_isPowerUpActive = true;

	switch (powerUpType) {
	case POWERUP_DISCARD_CARD:
		QMessageBox::information(this, "Discard Card",
			"Select a card from your hand to discard it.");
		break;

	case POWERUP_SKIP_TURN:
		// Folosește imediat power-up-ul
		usePowerUp(powerUpId);
		m_isPowerUpActive = false;
		m_activePowerUpId = -1;
		m_activePowerUpType = -1;
		break;

	case POWERUP_STEAL_CARD:
		showStealCardDialog();
		break;

	case POWERUP_PLACE_ANY_CARD:
		QMessageBox::information(this, "Wild Card",
			"Select a card from your hand, then select any pile to place it.");
		break;

	case POWERUP_NO_SCORE_LOSS:
		// Acest power-up se activează automat la pierderea jocului
		usePowerUp(powerUpId);
		m_isPowerUpActive = false;
		m_activePowerUpId = -1;
		m_activePowerUpType = -1;
		QMessageBox::information(this, "Safety Net",
			"Safety Net activated! You won't lose points if you lose this game.");
		break;

	default:
		qDebug() << "Unknown power-up type:" << powerUpType;
		m_isPowerUpActive = false;
		m_activePowerUpId = -1;
		m_activePowerUpType = -1;
		break;
	}
}

void GamePage::usePowerUp(int powerUpId, int targetPlayerId, int targetCardValue, int targetPileId)
{
	QJsonObject obj;
	obj["powerUpId"] = powerUpId;

	if (targetPlayerId != -1) {
		obj["targetPlayerId"] = targetPlayerId;
	}
	if (targetCardValue != -1) {
		obj["targetCardValue"] = targetCardValue;
	}
	if (targetPileId != -1) {
		obj["targetPileId"] = targetPileId;
	}

	QNetworkRequest req(QUrl(QString("http://localhost:18080/game/%1/player/%2/usepower")
		.arg(m_gameId).arg(m_playerId)));
	req.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

	QNetworkReply* reply = net->post(req, QJsonDocument(obj).toJson());

	connect(reply, &QNetworkReply::finished, this, [=]() {
		auto data = reply->readAll();
		QJsonDocument doc = QJsonDocument::fromJson(data);
		QJsonObject respObj = doc.object();

		qDebug() << "Power-up response:" << doc;

		if (respObj["success"].toBool()) {
			QString message = respObj["message"].toString();
			QString powerUpName = respObj["powerUpName"].toString();

			QMessageBox::information(this, "Power-up Used",
				QString("%1 used successfully!\n%2").arg(powerUpName).arg(message));

			// Refresh hand and equipped power-ups
			FetchHand();
			fetchEquippedPowerUps();

			// Dacă a fost Skip Turn, refresh game state
			if (powerUpId == POWERUP_SKIP_TURN) {
				fetchGameState();
			}
		}
		else {
			QString error = respObj["error"].toString();
			QMessageBox::warning(this, "Power-up Failed",
				QString("Failed to use power-up:\n%1").arg(error));
		}

		reply->deleteLater();
		});
}

void GamePage::showStealCardDialog()
{
	// Creează widget-ul pentru steal card dacă nu există
	if (!m_stealCardWidget) {
		m_stealCardWidget = new QWidget(this);
		m_stealCardWidget->setStyleSheet(
			"QWidget {"
			"    background-color: #FDEED9;"
			"    border: 2px solid #A98360;"
			"    border-radius: 10px;"
			"}"
			"QPushButton {"
			"    background-color: #FFADC6;"
			"    color: white;"
			"    border: none;"
			"    padding: 10px;"
			"    font-size: 14pt;"
			"    font-weight: bold;"
			"    border-radius: 5px;"
			"}"
			"QPushButton:hover {"
			"    background-color: #E34989;"
			"}"
			"QLabel {"
			"    color: #537c27;"
			"    font-size: 16pt;"
			"    font-weight: bold;"
			"}"
		);

		m_stealCardLayout = new QVBoxLayout(m_stealCardWidget);

		QLabel* title = new QLabel("Select a player to steal from:", m_stealCardWidget);
		title->setAlignment(Qt::AlignCenter);

		title->setStyleSheet(
			"color: #573c27;"       
			"font-size: 15pt;"      
			"font-weight: bold;"    
			"font-family: 'Arial';" 
		);
		m_stealCardLayout->addWidget(title);

		m_stealCardWidget->setLayout(m_stealCardLayout);
	}

	// Șterge butoanele vechi
	while (m_stealCardLayout->count() > 1) {
		QLayoutItem* item = m_stealCardLayout->takeAt(1);
		if (item->widget()) {
			delete item->widget();
		}
		delete item;
	}

	// Fetch players from game state
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
		QJsonArray players = obj["players"].toArray();

		for (const auto& playerVal : players) {
			QJsonObject playerObj = playerVal.toObject();
			int playerId = playerObj["id"].toInt();
			QString username = playerObj["username"].toString();

			// Skip current player
			if (playerId == m_playerId) {
				continue;
			}

			QPushButton* playerBtn = new QPushButton(username, m_stealCardWidget);
			connect(playerBtn, &QPushButton::clicked, [this, playerId, username]() {
				qDebug() << "Stealing from player:" << username << "ID:" << playerId;
				usePowerUp(m_activePowerUpId, playerId, -1, -1);
				hideStealCardDialog();
				m_isPowerUpActive = false;
				m_activePowerUpId = -1;
				m_activePowerUpType = -1;
				});

			m_stealCardLayout->addWidget(playerBtn);
		}

		// Adaugă buton de anulare
		QPushButton* cancelBtn = new QPushButton("Cancel", m_stealCardWidget);
		cancelBtn->setStyleSheet(
			"QPushButton {"
			"    background-color: #A98360;"
			"}"
			"QPushButton:hover {"
			"    background-color: #573c27;"
			"}"
		);
		connect(cancelBtn, &QPushButton::clicked, this, [this]() {
			hideStealCardDialog();
			m_isPowerUpActive = false;
			m_activePowerUpId = -1;
			m_activePowerUpType = -1;
			});
		m_stealCardLayout->addWidget(cancelBtn);

		// Poziționează widget-ul în centrul ecranului
		int w = 300;
		int h = 400;
		m_stealCardWidget->setGeometry(
			(this->width() - w) / 2,
			(this->height() - h) / 2,
			w, h
		);

		m_stealCardWidget->show();
		m_stealCardWidget->raise();

		reply->deleteLater();
		});
}

void GamePage::hideStealCardDialog()
{
	if (m_stealCardWidget) {
		m_stealCardWidget->hide();
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

