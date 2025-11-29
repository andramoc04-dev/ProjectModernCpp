#pragma once

#include <QDialog>
#include<QLabel>
#include "ui_ProfilePage.h"



class ProfilePage : public QDialog
{
	Q_OBJECT

public:
	ProfilePage(QWidget *parent = nullptr);
	~ProfilePage();



private:
	Ui::ProfilePageClass ui;
	
};

