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
#include <QMessageBox>

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

			QJsonArray powerUpsArray = obj["powerUps"].toArray();

			std::unordered_set<QString> powerUpsValues;
			std::unordered_map<QString, int> powerUpToInventoryId;

			for (const QJsonValue& powerUpValue : powerUpsArray) {
				QJsonObject powerUp = powerUpValue.toObject();

				QString powerUpId = QString::number(powerUp["powerUpId"].toInt());
				int inventoryId = powerUp["inventoryId"].toInt();
				if (powerUpToInventoryId.find(powerUpId) == powerUpToInventoryId.end()) {
					powerUpToInventoryId[powerUpId] = inventoryId;
				}

				powerUpsValues.insert(powerUpId);
			}
			showPowerUpOptions(powerUpsValues, powerUpToInventoryId);
		}
		reply->deleteLater();
		});
}


void InventoryPage::showPowerUpOptions(std::unordered_set<QString> availablePowerups,
	const std::unordered_map<QString, int>& powerUpToInventoryId)
{
	QLayoutItem* item;
	while ((item = ui.grid->takeAt(0)) != nullptr) {
		if (QWidget* w = item->widget()) {
			w->deleteLater();
		}
		delete item;
	}

	m_powerUpButtons.clear();

	ui.grid->setSpacing(10);
	ui.grid->setContentsMargins(10, 10, 10, 10);

	int index = 0;

	for (const auto& powerUpValue: availablePowerups) {
		
		QPushButton* button = new QPushButton(this);
		button->setFixedSize(100, 100);
		button->setCheckable(true);
		QString imgPath = QString(":/Client/resources/powerup%1.jpg").arg(powerUpValue);

		button->setStyleSheet(QString(
			"QPushButton {"
			"    border: 2px solid #333;"
			"    border-radius: 5px;"
			"	 border: 4px solid #FFADC6;"	
			"    background: url(%1) no-repeat center;"
			"}"
			"QPushButton:hover {"
			"    border-color: #666;"
			"}"
			"QPushButton:checked {"
			"    border: 4px solid #E34989;"
			"    background-color: #66BB6A;"
			"}"
		).arg(imgPath));

		m_powerUpButtons.append(button);

		int inventoryId = powerUpToInventoryId.at(powerUpValue);


		connect(button, &QPushButton::clicked, [this, powerUpValue, button, inventoryId]() {
			int powerUpId = powerUpValue.toInt();
			int selectedCount = getSelectedCount();

			if (button->isChecked() && selectedCount > MAX_EQUIPPED) {
				qDebug() << "Cannot select more than" << MAX_EQUIPPED << "power-ups!";
				button->setChecked(false);
				QMessageBox::warning(this, "Selection Limit",
					QString("You can only equip %1 power-ups at a time!").arg(MAX_EQUIPPED));
				return;
			}

			if (button->isChecked()) {
				qDebug() << "Equipping power-up:" << powerUpId;
				equipPowerUp(inventoryId);
			}
			else {
				qDebug() << "Unequipping power-up:" << powerUpId;
				unequipPowerUp(inventoryId);
			}
			});


		ui.grid->addWidget(button, 0, ++index, Qt::AlignCenter);

	}

	qDebug() << "Displayed" << availablePowerups.size() << "power-up instances in grid";
}

int InventoryPage::getSelectedCount() const
{
	int count = 0;
	for (const QPushButton* btn : m_powerUpButtons) {
		if (btn->isChecked()) {
			count++;
		}
	}
	return count;
}

void InventoryPage::equipPowerUp(int powerUpId)
{
	QString urlString = QString("http://127.0.0.1:18080/player/%1/powerups/%2/equip")
		.arg(m_playerId)
		.arg(powerUpId);

	QNetworkRequest request(urlString);
	request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

	QNetworkReply* reply = m_networkManager->post(request, QByteArray());

	connect(reply, &QNetworkReply::finished, [reply, powerUpId]() {
		if (reply->error() == QNetworkReply::NoError) {
			qDebug() << "Power-up" << powerUpId << "equipped successfully!";
		}
		else {
			qDebug() << "Error equipping power-up:" << reply->errorString();
		}
		reply->deleteLater();
		});
}

void InventoryPage::unequipPowerUp(int powerUpId)
{
	QString urlString = QString("http://127.0.0.1:18080/player/%1/powerups/%2/unequip")
		.arg(m_playerId)
		.arg(powerUpId);

	QNetworkRequest request(urlString);
	request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

	QNetworkReply* reply = m_networkManager->post(request, QByteArray());

	connect(reply, &QNetworkReply::finished, [reply, powerUpId]() {
		if (reply->error() == QNetworkReply::NoError) {
			qDebug() << "Power-up" << powerUpId << "unequipped successfully!";
		}
		else {
			qDebug() << "Error unequipping power-up:" << reply->errorString();
		}
		reply->deleteLater();
		});
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