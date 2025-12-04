#pragma once

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkRequest>
#include <QUrl>


#include <QDialog>
#include <QMessageBox>
#include <QLabel>
#include<QRegularExpression>
#include "ui_LoginPage.h"
#include "MenuPage.h"


class LoginPage : public QDialog
{
	Q_OBJECT

public:
	LoginPage(QWidget *parent = nullptr);
	~LoginPage();
	bool getLoggedIn();


private slots:
	
	void on_loginButton_clicked();
	void on_registerButton_clicked();

	void onRegisterReply(QNetworkReply* reply);
	void onLoginReply(QNetworkReply* reply);
private:
	Ui::LoginPageClass ui;
	QNetworkAccessManager* manager;
	bool loggedIn;
	MenuPage* menu;
};

