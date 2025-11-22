#pragma once

#include <QDialog>
#include "ui_ProfilePage.h"
#include<QLabel>

class ProfilePage : public QDialog
{
	Q_OBJECT

public:
	ProfilePage(QWidget *parent = nullptr);
	~ProfilePage();

private:
	Ui::ProfilePageClass ui;
};

