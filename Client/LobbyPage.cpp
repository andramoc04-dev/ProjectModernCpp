#include "LobbyPage.h"
#include "ui_LobbyPage.h"
#include "GamePage.h" // folosește GamePage existent
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QUrl>
#include <QDebug>

LobbyPage::LobbyPage(int playerId,const QString& playerName, const QString& serverBaseUrl, QWidget* parent)
    : QDialog(parent),
    ui(new Ui::LobbyPageClass),
    network(new QNetworkAccessManager(this)),
    pollTimer(new QTimer(this)),
    m_playerId(playerId),
    m_playerName(playerName),
    m_serverBase(serverBaseUrl)
{
    ui->setupUi(this);

    // trimite join imediat
    QUrl url(m_serverBase + "/join");
    QNetworkRequest req(url);
    req.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    QJsonObject obj;
    obj["id"] = m_playerId;
    QNetworkReply* reply = network->post(req, QJsonDocument(obj).toJson());
    connect(reply, &QNetworkReply::finished, this, &LobbyPage::onJoinReplyFinished);

    // pornim polling la fiecare 1000ms
    connect(pollTimer, &QTimer::timeout, this, &LobbyPage::pollStatus);
    pollTimer->start(1000);

    ui->statusLabel->setText("Aștept jucători...");
}

LobbyPage::~LobbyPage()
{
    delete ui;
}

void LobbyPage::onJoinReplyFinished()
{
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    if (!reply) return;
    if (reply->error() != QNetworkReply::NoError) {
        qWarning() << "Join failed:" << reply->errorString();
        if (ui->statusLabel) ui->statusLabel->setText("Eroare la join: " + reply->errorString());
    }
    else {
        if (ui->statusLabel) ui->statusLabel->setText("Te-ai alăturat lobby-ului. Așteaptă opponent...");
    }
    reply->deleteLater();
}

void LobbyPage::pollStatus()
{
    QString url = QString("%1/status/%2").arg(m_serverBase).arg(m_playerId);
    QUrl qurl(url);
    QNetworkRequest req(qurl);
    QNetworkReply* reply = network->get(req);
    connect(reply, &QNetworkReply::finished, this, &LobbyPage::onStatusReplyFinished);
}

void LobbyPage::onStatusReplyFinished()
{
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    if (!reply) return;

    if (reply->error() == QNetworkReply::NoError) {
        QByteArray bytes = reply->readAll();
        QJsonParseError err;
        QJsonDocument doc = QJsonDocument::fromJson(bytes, &err);
        if (err.error == QJsonParseError::NoError && doc.isObject()) {
            QJsonObject obj = doc.object();
            bool started = obj.value("started").toBool(false);
            if (started) {
                int gameId = obj.value("gameId").toInt(-1);
                int opponentId = obj.value("opponentId").toInt(-1);

                // oprește polling-ul
                pollTimer->stop();

                // pornește GamePage — adaptează constructorul dacă e necesar
                GamePage* gp = nullptr;
                // Dacă GamePage are constructor (int playerId, int gameId, int opponentId, QWidget*)
                // folosește-l; altfel adaptează după implementarea ta.
                gp = new GamePage(m_playerId,m_playerName, gameId, opponentId, nullptr);
                gp->show();

                // închide/hide lobby
                this->close();
            }
            else {
                ui->statusLabel->setText("Aștept jucător...");
            }
        }
        else {
            qWarning() << "JSON parse error in status:" << err.errorString();
        }
    }
    else {
        qWarning() << "Status request error:" << reply->errorString();
    }
    reply->deleteLater();
}
