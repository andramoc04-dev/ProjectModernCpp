#include "InventoryPage.h"
#include "GamePage.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QNetworkAccessManager>
#include <QUrl>
#include <QDebug>
#include <QJsonArray>
#include <QPushButton>
#include <QFile>

InventoryPage::InventoryPage(QWidget* parent)
	: QDialog(parent)
	, m_playerId(-1)
	, m_gameId(-1)
	, m_opponentId(-1)
	, m_timerExpired(false)
{
	ui.setupUi(this);
	m_networkManager = new QNetworkAccessManager(this);

	m_totalSeconds = 10;
	m_gameTimer.start(m_totalSeconds);

	qDebug() << "InventoryPage: Timer started for" << m_totalSeconds << "seconds";

	QCoreApplication::postEvent(this, new QEvent(QEvent::User));
}

InventoryPage::~InventoryPage()
{
}

void InventoryPage::setPlayerData(int playerId, const QString& playerName, int gameId, int opponentId)
{
	m_playerId = playerId;
	m_playerName = playerName;
	m_gameId = gameId;
	m_opponentId = opponentId;

	qDebug() << "InventoryPage - Player data set:" << playerId << playerName << gameId << opponentId;
}

void InventoryPage::fetchAvailablePowerUps()
{
	QString urlString = QString("http://127.0.0.1:18080/player/%1/powerups")
		.arg(m_playerId);
	QUrl url(urlString);

	QNetworkRequest request;
	request.setUrl(url);
	QNetworkReply* reply = m_networkManager->get(request);

	connect(reply, &QNetworkReply::finished, [this, reply]() {
		if (reply->error() == QNetworkReply::NoError) {
			QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());
			QJsonObject obj = doc.object();

			qDebug() << "Power-ups received for player" << obj["playerId"].toInt();
			qDebug() << "Total power-ups:" << obj["totalPowerUps"].toInt();

			QJsonArray powerUpsArray = obj["powerUps"].toArray();
			qDebug() << "Available power-ups count:" << powerUpsArray.size();

			std::vector<QString> powerUpsValues;
			for (const QJsonValue& powerUpValue : powerUpsArray) {
				QJsonObject powerUp = powerUpValue.toObject();

				// Get the type ID (1-5) for the image
				QString powerUpId = QString::number(powerUp["powerUpId"].toInt());
				int inventoryId = powerUp["inventoryId"].toInt();
				QString type = powerUp["type"].toString();
				QString name = powerUp["name"].toString();

				qDebug() << "  InventoryID:" << inventoryId
					<< "PowerUpID:" << powerUpId
					<< "Type:" << type
					<< "Name:" << name;

				// Push the powerUpId (for image), but we have unique instances now
				powerUpsValues.push_back(powerUpId);
			}
			showPowerUpOptions(powerUpsValues);
		}
		else {
			qDebug() << "Error fetching power-ups:" << reply->errorString();
		}
		reply->deleteLater();
		});
}


void InventoryPage::showPowerUpOptions(std::vector<QString> availablePowerups)
{
	QLayoutItem* item;
	while ((item = ui.grid->takeAt(0)) != nullptr) {
		if (QWidget* w = item->widget()) {
			w->deleteLater();
		}
		delete item;
	}

	ui.grid->setSpacing(10);
	ui.grid->setContentsMargins(10, 10, 10, 10);

	int buttonsPerRow = 5;
	int index = 0;

	for (const auto& powerUpValue : availablePowerups) {
		QPushButton* button = new QPushButton(this);
		button->setFixedSize(100, 100);

		QString imgPath = QString(":/Client/resources/powerup%1.jpg").arg(powerUpValue);

		qDebug() << "Loading image from:" << imgPath;  // Debug: check image path

		button->setStyleSheet(QString(
			"QPushButton {"
			"    border: 2px solid #333;"
			"    border-radius: 5px;"
			"    background-image: url(%1);"
			"    background-repeat: no-repeat;"
			"    background-position: center;"
			"    background-color: #E34989;"
			"}"
			"QPushButton:hover {"
			"    border: 2px solid #666;"
			"}"
			"QPushButton:pressed {"
			"    border: 2px solid #999;"
			"}"
		).arg(imgPath));


		connect(button, &QPushButton::clicked, [this, powerUpValue]() {
			qDebug() << "Power-up selected:" << powerUpValue;
			});

		int row = index / buttonsPerRow;
		int col = index % buttonsPerRow;

		ui.grid->addWidget(button, row, col, Qt::AlignCenter);

		index++;
	}

	qDebug() << "Displayed" << availablePowerups.size() << "power-up instances in grid";
}


bool InventoryPage::event(QEvent* e)
{
	if (e->type() == QEvent::User) {
		updateTimerUI();

		if (m_gameTimer.running() && !m_gameTimer.expired()) {
			QCoreApplication::postEvent(this, new QEvent(QEvent::User));
		}
		return true;
	}

	return QDialog::event(e);
}

void InventoryPage::updateTimerUI()
{
	if (!m_gameTimer.running()) {
		return;
	}

	const int elapsed = m_gameTimer.elapsedSeconds();
	const int remaining = std::max(0, m_totalSeconds - elapsed);

	// Update UI to show remaining time (if you have a label for it)
	// ui.timerLabel->setText(QString("Time remaining: %1s").arg(remaining));

	qDebug() << "InventoryPage - Time remaining:" << remaining << "seconds";

	// Check if timer expired
	if (m_gameTimer.expired() && !m_timerExpired) {
		m_timerExpired = true;
		onTimerExpired();
	}
}

void InventoryPage::onTimerExpired()
{
	qDebug() << "InventoryPage: Timer expired - transitioning to GamePage";

	if (m_playerId < 0 || m_gameId < 0) {
		this->close();
		return;
	}

	GamePage* gamePage = new GamePage(m_playerId, m_playerName, m_gameId, m_opponentId, nullptr);
	gamePage->show();

	this->close();
	this->deleteLater();
}