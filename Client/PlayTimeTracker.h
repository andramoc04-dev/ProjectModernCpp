#pragma once
#include <QObject>
#include <QTimer>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <chrono>

class PlayTimeTracker : public QObject
{
    Q_OBJECT
public:
    explicit PlayTimeTracker(QObject* parent = nullptr);
    ~PlayTimeTracker();

    void startTracking(int playerId, const QString& serverUrl);
    void stopAndSync();
    bool isTracking() const;
    int getCurrentPlayerId() const { return m_playerId; }

private slots:
    void onBackupSync();
    void onSyncReplyFinished();

private:
    void syncPlayTime(int seconds);
    int getElapsedSeconds() const;

private:
    QNetworkAccessManager* m_network;
    QTimer* m_backupTimer;


    std::chrono::steady_clock::time_point m_sessionStart;


    int m_totalSyncedSeconds{ 0 };

    int m_playerId{ -1 };
    QString m_serverUrl;
    bool m_isTracking{ false };

    static constexpr int BACKUP_INTERVAL_MS = 5 * 60 * 1000;
};
