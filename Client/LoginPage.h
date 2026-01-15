#pragma once

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkRequest>
#include <QUrl>

#include <QDialog>
#include <QMessageBox>
#include <QRegularExpression>
#include "ui_LoginPage.h"
#include "MenuPage.h"
#include "PlayTimeTracker.h"

namespace Ui { class LoginPageClass; }

class LoginPage : public QDialog
{
    Q_OBJECT

public:
    LoginPage(QWidget* parent = nullptr);
    ~LoginPage();
    static PlayTimeTracker* getPlayTimeTracker();

private slots:
    void on_loginButton_clicked();
    void on_registerButton_clicked();

    void onRegisterReply(QNetworkReply* reply);
    void onLoginReply(QNetworkReply* reply);
private:
    bool validateInput(const QString& username, const QString& password);
    Ui::LoginPageClass* ui;
    QNetworkAccessManager* manager;
    bool loggedIn;
    static PlayTimeTracker* s_playTimeTracker;
    MenuPage* menu;
};
