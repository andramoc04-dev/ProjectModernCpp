#pragma once

#include <QDialog>
#include "ui_LoginPage.h"

class LoginPage : public QDialog
{
	Q_OBJECT

public:
	LoginPage(QWidget *parent = nullptr);
	~LoginPage();

private:
	Ui::LoginPageClass ui;
};

