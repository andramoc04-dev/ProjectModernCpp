#include "PlayTimeTracker.h"
#include <QJsonObject>
#include <QJsonDocument>
#include <QNetworkRequest>
#include <QUrl>
#include <QDebug>

PlayTimeTracker::PlayTimeTracker(QObject* parent)
    : QObject(parent)
    , m_network(new QNetworkAccessManager(this))
    , m_backupTimer(new QTimer(this))
{
    m_backupTimer->setInterval(BACKUP_INTERVAL_MS);
    connect(m_backupTimer, &QTimer::timeout, this, &PlayTimeTracker::onBackupSync);
}

PlayTimeTracker::~PlayTimeTracker()
{
    if (m_isTracking) {
        stopAndSync();
    }
}

void PlayTimeTracker::startTracking(int playerId, const QString& serverUrl)
{
    if (m_isTracking) {
        qWarning() << "PlayTimeTracker: Already tracking!";
        return;
    }

    m_playerId = playerId;
    m_serverUrl = serverUrl;
    m_sessionStart = std::chrono::steady_clock::now();
    m_lastSync = m_sessionStart;
    m_isTracking = true;

    // Porne?te backup timer-ul
    m_backupTimer->start();

    qDebug() << "PlayTimeTracker: Started tracking for player" << playerId;
}

void PlayTimeTracker::stopAndSync()
{
    if (!m_isTracking) {
        return;
    }

    m_backupTimer->stop();

    // Calculeaz? timpul total de la ultima sincronizare
    const int secondsToSync = getElapsedSeconds();

    if (secondsToSync > 0) {
        syncPlayTime(secondsToSync);
    }

    m_isTracking = false;
    qDebug() << "PlayTimeTracker: Stopped tracking. Final sync:" << secondsToSync << "seconds";
}

bool PlayTimeTracker::isTracking() const
{
    return m_isTracking;
}

void PlayTimeTracker::onBackupSync()
{
    if (!m_isTracking) {
        return;
    }

    const int secondsSinceLastSync = getElapsedSeconds();

    if (secondsSinceLastSync > 0) {
        qDebug() << "PlayTimeTracker: Backup sync -" << secondsSinceLastSync << "seconds";
        syncPlayTime(secondsSinceLastSync);
        m_lastSync = std::chrono::steady_clock::now();
    }
}

int PlayTimeTracker::getElapsedSeconds() const
{
    using namespace std::chrono;
    const auto now = steady_clock::now();
    const auto elapsed = duration_cast<seconds>(now - m_lastSync);
    return static_cast<int>(elapsed.count());
}

void PlayTimeTracker::syncPlayTime(int seconds)
{
    if (m_playerId < 0 || m_serverUrl.isEmpty()) {
        qWarning() << "PlayTimeTracker: Invalid player ID or server URL";
        return;
    }

    QJsonObject payload;
    payload["seconds"] = seconds;

    const QJsonDocument doc(payload);
    const QByteArray body = doc.toJson(QJsonDocument::Compact);

    const QUrl url(m_serverUrl + "/player/" + QString::number(m_playerId) + "/updateplaytime");
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QNetworkReply* reply = m_network->post(request, body);
    connect(reply, &QNetworkReply::finished, this, &PlayTimeTracker::onSyncReplyFinished);
}

void PlayTimeTracker::onSyncReplyFinished()
{
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    if (!reply) return;

    if (reply->error() != QNetworkReply::NoError) {
        qWarning() << "PlayTimeTracker: Sync failed -" << reply->errorString();
    }
    else {
        const int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        if (statusCode == 200) {
            qDebug() << "PlayTimeTracker: Sync successful";
        }
        else {
            qWarning() << "PlayTimeTracker: Sync returned status" << statusCode;
        }
    }

    reply->deleteLater();
}