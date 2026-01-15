#pragma once

#include <QDialog>
#include <QNetworkAccessManager>
#include <atomic>
#include <thread>

namespace Ui { class LobbyPageClass; }

class LobbyPage : public QDialog
{
    Q_OBJECT
public:
    LobbyPage(int playerId, const QString& playerName, const QString& serverBaseUrl, QWidget* parent = nullptr);
    ~LobbyPage();

private slots:
    void onJoinReplyFinished();
    void pollStatus();
    void onStatusReplyFinished();
private:

    Ui::LobbyPageClass* ui;
    QNetworkAccessManager* network;

    std::atomic_bool m_pollRunning{ false };
    std::thread m_pollThread;

    int m_playerId;
    QString m_serverBase;
    QString m_playerName;
};
