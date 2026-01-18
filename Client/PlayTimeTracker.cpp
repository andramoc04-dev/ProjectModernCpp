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
    
    if (m_isTracking && m_playerId != playerId) {
        qDebug() << "PlayTimeTracker: Switching from player" << m_playerId
            << "to player" << playerId;
        stopAndSync();
    }

    if (m_isTracking && m_playerId == playerId) {
        qWarning() << "PlayTimeTracker: Already tracking for player" << playerId;
        return;
    }

    m_playerId = playerId;
    m_serverUrl = serverUrl;

    
    m_sessionStart = std::chrono::steady_clock::now();
    m_totalSyncedSeconds = 0;
    m_isTracking = true;

    
    m_backupTimer->start();

    qDebug() << "PlayTimeTracker: Started tracking for player" << playerId;
}

void PlayTimeTracker::stopAndSync()
{
    if (!m_isTracking) {
        return;
    }

    m_backupTimer->stop();

    const int totalSecondsInSession = getElapsedSeconds();

    
    const int secondsToSync = totalSecondsInSession - m_totalSyncedSeconds;

    if (secondsToSync > 0) {
        qDebug() << "PlayTimeTracker: Final sync :" << secondsToSync << "seconds"
            << "(Total session:" << totalSecondsInSession << "seconds)";
        syncPlayTime(secondsToSync);
    }

    m_isTracking = false;
    m_totalSyncedSeconds = 0; 
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

    
    const int totalSecondsInSession = getElapsedSeconds();

    
    const int secondsToSync = totalSecondsInSession - m_totalSyncedSeconds;

    if (secondsToSync > 0) {
        qDebug() << "PlayTimeTracker: Backup sync :" << secondsToSync << "seconds"
            << "(Total session so far:" << totalSecondsInSession << "seconds)";

        syncPlayTime(secondsToSync);

        
        m_totalSyncedSeconds = totalSecondsInSession;
    }
}

int PlayTimeTracker::getElapsedSeconds() const
{
    using namespace std::chrono;

    
    const auto now = steady_clock::now();
    const auto elapsed = duration_cast<seconds>(now - m_sessionStart);

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

    qDebug() << "PlayTimeTracker: Sending sync request for" << seconds << "seconds to player" << m_playerId;

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
            
            QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());
            if (doc.isObject()) {
                QJsonObject obj = doc.object();
                int totalMinutes = obj["totalMinutes"].toInt();
                QString formattedTime = obj["formattedTime"].toString();

                qDebug() << "PlayTimeTracker: Sync successful for player" << m_playerId
                    << "- Total time:" << formattedTime << "(" << totalMinutes << "minutes)";
            }
        }
        else {
            qWarning() << "PlayTimeTracker: Sync returned status" << statusCode;
        }
    }

    reply->deleteLater();
}

