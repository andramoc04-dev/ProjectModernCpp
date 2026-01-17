#include "EndgamePage.h"
#include "MenuPage.h"
#include <QJsonObject>
#include <QJsonDocument>
#include <QNetworkRequest>
#include <QUrl>
#include <QMessageBox>
#include <QDebug>

EndgamePage::EndgamePage(int playerId, const QString& playerName, int gameId,
    QString gameEnding, int unusedCardsCount,
    const QString& serverUrl, bool hasScoreLossProtection ,
    QWidget* parent)
    : QDialog(parent)
    , m_network(new QNetworkAccessManager(this))
    , m_playerId(playerId)
    , m_playerName(playerName)
    , m_gameId(gameId)
    , m_gameEnding(gameEnding)
    , m_unusedCardsCount(unusedCardsCount)
    , m_serverUrl(serverUrl)
{
    ui.setupUi(this);

    // Seteaz? background-ul în func?ie de rezultat
    if (gameEnding == "won") {
        this->setStyleSheet("border-image: url(:/Client/resources/win_background.jpeg) 0 0 0 0 stretch stretch;");
    }
    else {
        this->setStyleSheet("border-image: url(:/Client/resources/lose_background.jpeg) 0 0 0 0 stretch stretch;");
    }
}

EndgamePage::~EndgamePage()
{
}

void EndgamePage::showEvent(QShowEvent* event)
{
    QDialog::showEvent(event);

    // Trimite rezultatul jocului doar o singur? dat?
    if (!m_resultSubmitted) {
        submitGameResult();
    }
}

void EndgamePage::submitGameResult()
{
    qDebug() << "Submitting game result for player" << m_playerId
        << "- Result:" << m_gameEnding
        << "- Unused cards:" << m_unusedCardsCount;

    if (m_gameEnding == "won") {
        recordWin();
    }
    else {
        recordLoss();
    }
}

void EndgamePage::recordWin()
{
    const QUrl url(m_serverUrl + "/game/" + QString::number(m_gameId) +
        "/player/" + QString::number(m_playerId) + "/win");

    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    qDebug() << "Recording win at:" << url.toString();

    QNetworkReply* reply = m_network->post(request, QByteArray());
    connect(reply, &QNetworkReply::finished, this, &EndgamePage::onWinReplyFinished);
}

void EndgamePage::recordLoss()
{
    const QUrl url(m_serverUrl + "/game/" + QString::number(m_gameId) +
        "/player/" + QString::number(m_playerId) + "/loss");

    QJsonObject payload;
    payload["unusedCards"] = m_unusedCardsCount;

    const QJsonDocument doc(payload);
    const QByteArray body = doc.toJson(QJsonDocument::Compact);

    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    qDebug() << "Recording loss at:" << url.toString();
    qDebug() << "Unused cards to record:" << m_unusedCardsCount;

    QNetworkReply* reply = m_network->post(request, body);
    connect(reply, &QNetworkReply::finished, this, &EndgamePage::onLossReplyFinished);
}

void EndgamePage::onWinReplyFinished()
{
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    if (!reply) return;

    if (reply->error() != QNetworkReply::NoError) {
        qWarning() << "Failed to record win:" << reply->errorString();
        reply->deleteLater();
        return;
    }

    const int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();

    if (statusCode == 200) {
        qDebug() << "Win recorded successfully";
        m_resultSubmitted = true;

        // Dup? ce am înregistrat rezultatul, recalcul?m scorul
        recalculateScore();
    }
    else {
        const QByteArray body = reply->readAll();
        qWarning() << "Failed to record win. HTTP" << statusCode << body;
    }

    reply->deleteLater();
}

void EndgamePage::onLossReplyFinished()
{
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    if (!reply) return;

    if (reply->error() != QNetworkReply::NoError) {
        qWarning() << "Failed to record loss:" << reply->errorString();
        reply->deleteLater();
        return;
    }

    const int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();

    if (statusCode == 200) {
        qDebug() << "Loss recorded successfully";
        m_resultSubmitted = true;

        // Dup? ce am înregistrat rezultatul, recalcul?m scorul
        recalculateScore();
    }
    else {
        const QByteArray body = reply->readAll();
        qWarning() << "Failed to record loss. HTTP" << statusCode << body;
    }

    reply->deleteLater();
}

void EndgamePage::recalculateScore()
{
    const QUrl url(m_serverUrl + "/player/" + QString::number(m_playerId) + "/recalculate-score");

    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    qDebug() << "Recalculating score at:" << url.toString();

    QNetworkReply* reply = m_network->post(request, QByteArray());
    connect(reply, &QNetworkReply::finished, this, &EndgamePage::onRecalculateScoreFinished);
}

void EndgamePage::onRecalculateScoreFinished()
{
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    if (!reply) return;

    if (reply->error() != QNetworkReply::NoError) {
        qWarning() << "Failed to recalculate score:" << reply->errorString();
        reply->deleteLater();
        return;
    }

    const int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    const QByteArray body = reply->readAll();

    if (statusCode == 200) {
        QJsonDocument doc = QJsonDocument::fromJson(body);
        if (doc.isObject()) {
            QJsonObject obj = doc.object();
            int newScore = obj["newScore"].toInt();
            int wins = obj["wins"].toInt();
            int gamesPlayed = obj["gamesPlayed"].toInt();

            qDebug() << "Score recalculated successfully.";
            qDebug() << "New score:" << newScore;
            qDebug() << "Wins:" << wins << "/ Games played:" << gamesPlayed;

            m_scoreRecalculated = true;

            // Op?ional: Afi?eaz? scorul nou pe UI dac? ai un label pentru asta
            // if (auto* scoreLabel = findChild<QLabel*>("newScoreLabel")) {
            //     scoreLabel->setText(QString("Your score: %1").arg(newScore));
            // }
        }
    }
    else {
        qWarning() << "Failed to recalculate score. HTTP" << statusCode << body;
    }

    reply->deleteLater();
}

void EndgamePage::onBackToMenuClicked()
{
    // Întoarce-te la meniu
    MenuPage* menuPage = new MenuPage(m_playerId, m_playerName, nullptr);
    menuPage->show();
    this->close();
    this->deleteLater();
}