#pragma once

#include <QDialog>
#include<QLabel>
#include "ui_ProfilePage.h"
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrl>
#include <QJsonObject>
#include <QJsonDocument>
#include <QDebug>
#include <QMessageBox>


class ProfilePage : public QDialog
{
	Q_OBJECT

public:
    ProfilePage(int playerId, const QString& serverUrl, QWidget* parent = nullptr);
    ~ProfilePage();

private slots:
    void onProfileReplyFinished();

private:
    Ui::ProfilePageClass ui;
    QNetworkAccessManager* network;
    int m_playerId;
    QString m_serverUrl;

	
};

