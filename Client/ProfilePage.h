#pragma once

#include <QDialog>
#include "ui_ProfilePage.h"
#include <QNetworkAccessManager>
//#include <QNetworkRequest>
#include <QNetworkReply>
//#include <QUrl>



class ProfilePage : public QDialog
{
	Q_OBJECT

public:
    ProfilePage(int playerId, const QString& serverUrl, QWidget* parent = nullptr);
    ProfilePage(int playerId, const QString& playerName, const QString& serverUrl, QWidget* parent = nullptr);
    ~ProfilePage();

private slots:
    void onProfileReplyFinished();
    void onBackButtonClicked();

private:
    Ui::ProfilePageClass* ui;
    QNetworkAccessManager* network;
    int m_playerId;
    QString m_serverUrl;
    QString m_playerName;

	
};

