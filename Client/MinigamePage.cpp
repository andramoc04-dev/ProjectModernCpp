#include "MinigamePage.h"
#include "InventoryPage.h"
#include <QCloseEvent>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QUrl>
#include <QDebug>

MinigamePage::MinigamePage(QWidget* parent)
    : QDialog(parent)
{
    ui.setupUi(this);
    RunMinigame();
}

MinigamePage::~MinigamePage()
{
}

bool MinigamePage::event(QEvent* e)
{
    if (e->type() == QEvent::User) {
        updateTimerUi();
        if (m_gameTimer.running() && !m_gameTimer.expired()) {
            QCoreApplication::postEvent(this, new QEvent(QEvent::User));
        }
        return true;
    }

    return QDialog::event(e);
}

void MinigamePage::updateTimerUi()
{
    if (!ui.m_timerLabel) return;

    const int elapsed = m_gameTimer.elapsedSeconds();
    const int remaining = std::max(0, m_totalSeconds - elapsed);
    ui.m_timerLabel->setText(QString("Time: %1").arg(remaining));

    if (m_gameTimer.expired() && !m_finishedEmitted) {
        m_finishedEmitted = true;

        int playerId = property("playerId").toInt();
        QString playerName = property("playerName").toString();
        int gameId = property("gameId").toInt();

        // Submit minigame score and award power-ups
        submitMinigameScore(playerId, m_counter, playerName, gameId);


       
    }

}

void MinigamePage::RunMinigame()
{
    m_counter = 0;

    QGridLayout* layout = new QGridLayout(this);

    layout->addWidget(ui.m_timerLabel, 0, 0, 1, 2);
    layout->addWidget(ui.Score, 0, 2, 1, 1);
    layout->addWidget(ui.ScoreNumber, 0, 3, 1, 1);

    ui.m_timerLabel->setText("Time: 10");
    ui.ScoreNumber->setText("0");

    const QString style =
        "QPushButton {"
        "    border: none;"
        "    background-image: url(:/Client/resources/CardPlacer.png);"
        "}";

    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 5; j++) {
            QPushButton* button = new QPushButton(this);
            button->setStyleSheet(style);
            button->setFixedSize(80, 80);
            layout->addWidget(button, i + 1, j);

            connect(button, &QPushButton::clicked, this, [this, button]() {
                m_counter++;
                ui.ScoreNumber->setText(QString::number(m_counter));
                button->hide();
                });
        }
    }

    setLayout(layout);

    m_totalSeconds = 10;
    m_gameTimer.start(m_totalSeconds);
    QCoreApplication::postEvent(this, new QEvent(QEvent::User));
}

void MinigamePage::submitMinigameScore(int playerId, int score, const QString& playerName, int gameId)
{
    QNetworkAccessManager* networkManager = new QNetworkAccessManager(this);

    QUrl url("http://127.0.0.1:18080/minigame/complete");
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QJsonObject json;
    json["playerId"] = playerId;
    json["score"] = score;

    QJsonDocument doc(json);
    QByteArray data = doc.toJson();

    QNetworkReply* reply = networkManager->post(request, data);

    connect(reply, &QNetworkReply::finished, [this, reply, score, playerId, playerName, gameId]() {
        if (reply->error() == QNetworkReply::NoError) {
            QJsonDocument responseDoc = QJsonDocument::fromJson(reply->readAll());
            QJsonObject responseObj = responseDoc.object();

            bool rewardEarned = responseObj["rewardEarned"].toBool();
            if (rewardEarned) {
                QString powerUpType = responseObj["powerUpType"].toString();
                qDebug() << "Minigame reward earned! PowerUp:" << powerUpType;
                qDebug() << "Score:" << score;
            }
            else {
                qDebug() << "No reward earned. Score:" << score;
            }
        }
        else {
            qDebug() << "Error submitting minigame score:" << reply->errorString();
        }

        reply->deleteLater();

        InventoryPage* inventoryPage = new InventoryPage();
        inventoryPage->setAttribute(Qt::WA_DeleteOnClose);
        inventoryPage->setPlayerData(playerId, playerName, gameId);
        inventoryPage->fetchAvailablePowerUps();
        inventoryPage->show();

        this->close();
        this->deleteLater();
        });
}

void MinigamePage::closeEvent(QCloseEvent* event)
{

    event->accept();
}

