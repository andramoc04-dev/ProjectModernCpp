#pragma once

#include <QDialog>
#include <QMessageBox>
#include <QLabel>
#include<QRegularExpression>
#include "ui_LoginPage.h"

class LoginPage : public QDialog
{
	Q_OBJECT

public:
	LoginPage(QWidget *parent = nullptr);
	~LoginPage();



private slots:
	
	void on_loginButton_clicked();
	void on_registerButton_clicked();
private:
	Ui::LoginPageClass ui;

	
};

