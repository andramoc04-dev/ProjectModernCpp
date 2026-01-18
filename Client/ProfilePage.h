#pragma once

#include <QDialog>
#include "ui_ProfilePage.h"
#include <QNetworkAccessManager>
#include <QNetworkReply>



class ProfilePage : public QDialog
{
	Q_OBJECT

public:
    ProfilePage(int playerId, const QString& serverUrl, QWidget* parent = nullptr);
    ProfilePage(int playerId, const QString& playerName, const QString& serverUrl, QWidget* parent = nullptr);
    ~ProfilePage();

protected:
    void showEvent(QShowEvent* event) override;

private slots:
    void onProfileReplyFinished();
    void onBackButtonClicked();

private:
    void fetchProfileData(); 
    void updateUIWithProfileData(const QJsonObject& data); 


private:
    Ui::ProfilePageClass* ui;
    QNetworkAccessManager* network;
    int m_playerId;
    QString m_serverUrl;
    QString m_playerName;

	
};

