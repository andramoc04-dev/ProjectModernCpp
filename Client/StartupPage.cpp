#include "StartupPage.h"

StartupPage::StartupPage(QWidget *parent)
	: QDialog(parent), profilepage(nullptr)
{
	ui.setupUi(this);
	connect(ui.profileButton, &QPushButton::clicked, this, &StartupPage::openProfilePage);

}

StartupPage::~StartupPage()
{}

void StartupPage::openProfilePage()
{
    if (!profilepage)
        profilepage = new ProfilePage(this);  

    profilepage->show();   
    this->hide();          
}

