#include "LobbyPage.h"
#include "ui_LobbyPage.h"
#include "GamePage.h" // folosește GamePage existent
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QUrl>
#include <QDebug>
#include <QMetaObject>

#include <chrono>

LobbyPage::LobbyPage(int playerId,const QString& playerName, const QString& serverBaseUrl, QWidget* parent)
 : QDialog(parent),
 ui(new Ui::LobbyPageClass),
 network(new QNetworkAccessManager(this)),
 m_playerId(playerId),
 m_playerName(playerName),
 m_serverBase(serverBaseUrl)
{
 ui->setupUi(this);

 QUrl url(m_serverBase + "/join");
 QNetworkRequest req(url);
 req.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
 QJsonObject obj;
 obj["id"] = m_playerId;
 QNetworkReply* reply = network->post(req, QJsonDocument(obj).toJson());
 connect(reply, &QNetworkReply::finished, this, &LobbyPage::onJoinReplyFinished);

 ui->statusLabel->setText("Aștept jucători...");

 m_pollRunning = true;
 m_pollThread = std::thread([this]() {
 while (m_pollRunning.load()) {
 std::this_thread::sleep_for(std::chrono::milliseconds(1000));
 if (!m_pollRunning.load()) break;

 QMetaObject::invokeMethod(this, "pollStatus", Qt::QueuedConnection);
 }
 });
}

LobbyPage::~LobbyPage()
{
 m_pollRunning = false;
 if (m_pollThread.joinable()) {
 m_pollThread.join();
 }

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
 const QString url = QString("%1/status/%2").arg(m_serverBase).arg(m_playerId);
 const QUrl qurl(url);
 QNetworkRequest req(qurl);
 QNetworkReply* reply = network->get(req);
 connect(reply, &QNetworkReply::finished, this, &LobbyPage::onStatusReplyFinished);
}

void LobbyPage::onStatusReplyFinished()
{
 QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
 if (!reply) return;

 if (reply->error() == QNetworkReply::NoError) {
 const QByteArray bytes = reply->readAll();
 QJsonParseError err;
 QJsonDocument doc = QJsonDocument::fromJson(bytes, &err);
 if (err.error == QJsonParseError::NoError && doc.isObject()) {
 const QJsonObject obj = doc.object();
 bool started = obj.value("started").toBool(false);
 if (started) {
 int gameId = obj.value("gameId").toInt(-1);
 int opponentId = obj.value("opponentId").toInt(-1);

 m_pollRunning = false;

 GamePage* gp = new GamePage(m_playerId, m_playerName, gameId, opponentId, nullptr);
 gp->show();

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
