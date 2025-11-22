#pragma once

#include <QDialog>
#include "ui_StartupPage.h"
#include <QMessageBox>
#include <QLabel>
#include "ProfilePage.h"

class StartupPage : public QDialog
{
	Q_OBJECT

public:
	StartupPage(QWidget *parent = nullptr);
	~StartupPage();

private:
	Ui::StartupPageClass ui;
	ProfilePage* profilepage;

private slots:
	void openProfilePage();
};

