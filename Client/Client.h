#pragma once
#define NOMINMAX
#include <QtWidgets/QMainWindow>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMessageBox>
#include <QNetworkRequest>
#include <QUrl>
#include "ui_Client.h"


class Client : public QMainWindow
{
    Q_OBJECT

public:
    Client(QWidget* parent = nullptr);
    ~Client();

private slots:
    void on_btnShowPlayer_clicked();       // slot pentru buton
    void onPlayerReply(QNetworkReply* reply); // slot pentru răspuns HTTP

private:
    Ui::ClientClass ui;
    QNetworkAccessManager* manager;
};