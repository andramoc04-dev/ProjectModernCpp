#pragma once

#include <QDialog>
#include <QNetworkAccessManager>
#include <QTimer>

namespace Ui { class LobbyPageClass; }

class LobbyPage : public QDialog
{
    Q_OBJECT
public:
    LobbyPage(int playerId, const QString& serverBaseUrl, QWidget* parent = nullptr);
    ~LobbyPage();

private slots:
    void onJoinReplyFinished();
    void pollStatus();
    void onStatusReplyFinished();

private:
    Ui::LobbyPageClass* ui;
    QNetworkAccessManager* network;
    QTimer* pollTimer;
    int m_playerId;
    QString m_serverBase;
};
