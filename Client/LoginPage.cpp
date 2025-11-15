#include "LoginPage.h"

LoginPage::LoginPage(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);
	ui.Password->setEchoMode(QLineEdit::Password);
}

LoginPage::~LoginPage()
{}

void LoginPage::on_loginButton_clicked()
{
	QString username = ui.Username->text();
	QString password = ui.Password->text();

	QRegularExpression usernameRegex("^[A-Za-z][A-Za-z0-9]*$");
	QRegularExpression passwordRegex("^.{8,}$");

    if (!usernameRegex.match(username).hasMatch()) {
        QMessageBox::warning(this, "Eroare",
            "Invalid Username!\n Only digits and letters are accepted and the username should always start with a letter!");
        return;
    }
    
    
    if (!passwordRegex.match(password).hasMatch()) {
        QMessageBox::warning(this, "Eroare",
            "Invalid Password!\n You need 8 characters(letters, digits or symbols like @!#)");
        return;
    }
    QMessageBox::information(this, "Login", "Login succesful!!!");
    accept();
}







