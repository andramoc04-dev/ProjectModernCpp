#include "LoginPage.h"
#include <QDebug>

LoginPage::LoginPage(QWidget* parent)
    : QDialog(parent),
    ui(new Ui::LoginPageClass),
    manager(new QNetworkAccessManager(this)),
    loggedIn(false),
    menu(nullptr)
{
    ui->setupUi(this);
    ui->Password->setEchoMode(QLineEdit::Password);
}

LoginPage::~LoginPage()
{
}


void LoginPage::on_registerButton_clicked()
{
    const QString username = ui->Username->text().trimmed();
    const QString password = ui->Password->text();


    if (!validateInput(username, password)) {
        return;
    }

    QJsonObject root;
    root["name"] = username;
    root["password"] = password;

    QJsonDocument doc(root);
    QByteArray payload = doc.toJson(QJsonDocument::Compact);

    QUrl url("http://localhost:18080/addplayer");
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("Accept", "application/json");

    QNetworkReply* reply = manager->post(request, payload);
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        this->onRegisterReply(reply);
        });
    ui->registerButton->setEnabled(false);
}

void LoginPage::on_loginButton_clicked()
{
    const QString username = ui->Username->text();
    const QString password = ui->Password->text();


    if (!validateInput(username, password)) {
        return;
    }

    QJsonObject root;
    root["name"] = username;
    root["password"] = password;

    QJsonDocument doc(root);
    const QByteArray payload = doc.toJson(QJsonDocument::Compact);

    QUrl url("http://localhost:18080/login");
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("Accept", "application/json");

    QNetworkReply* reply = manager->post(request, payload);
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        this->onLoginReply(reply);
        });
    ui->loginButton->setEnabled(false);
}

void LoginPage::onRegisterReply(QNetworkReply* reply)
{
    ui->registerButton->setEnabled(true);

    if (reply->error() != QNetworkReply::NoError) {
        QMessageBox::warning(this, "Eroare rețea", reply->errorString());
        reply->deleteLater();
        return;
    }

    QVariant sc = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
    int statusCode = sc.isValid() ? sc.toInt() : -1;

    const QByteArray body = reply->readAll();
    const QString bodyStr = QString::fromUtf8(body);
    qDebug() << "Register reply. HTTP:" << statusCode << "Body:" << bodyStr;

    if (statusCode == 201) {
        QMessageBox::information(this, "Succes", "Cont creat cu succes!");
    }
    else if (statusCode == 409) {
        QString msg = "User already exists.";
        QJsonParseError err;
        const QJsonDocument doc = QJsonDocument::fromJson(body, &err);
        if (err.error == QJsonParseError::NoError && doc.isObject()) {
            const QJsonObject obj = doc.object();
            if (obj.contains("error")) msg = obj.value("error").toString();
        }
        else if (!bodyStr.isEmpty()) {
            msg = bodyStr;
        }
        QMessageBox::warning(this, "Eroare", msg);
    }
    else if (statusCode == 400) {
        QMessageBox::warning(this, "Eroare", "Bad request (server rejected payload).");
    }
    else {
        QMessageBox::warning(this, "Eroare server",
            QString("HTTP %1\n\n%2").arg(statusCode).arg(bodyStr));
    }

    reply->deleteLater();
}

void LoginPage::onLoginReply(QNetworkReply* reply)
{
    ui->loginButton->setEnabled(true);

    if (reply->error() != QNetworkReply::NoError) {
        QMessageBox::warning(this, "Eroare rețea", reply->errorString());
        reply->deleteLater();
        return;
    }

    int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    const QByteArray body = reply->readAll();
    const QJsonDocument doc = QJsonDocument::fromJson(body);

    if (statusCode == 200 && doc.isObject()) {
        const QJsonObject obj = doc.object();
        const QString name = obj["name"].toString();
        int score = obj["score"].toInt();

        // extragem ID din răspunsul serverului
        int id = -1;
        if (obj.contains("id")) {
            id = obj["id"].toInt(-1);
        }

        QMessageBox::information(this, "Login", "Login successful!\nWelcome " + name);
        loggedIn = true;

        // cream MenuPage, setam playerId si o aratam
        menu = new MenuPage(nullptr);
        if (id >= 0) {
            menu->setPlayerId(id);
          
        }
        else {
            // dacă serverul nu a trimis id, poți decide: fie refuzi, fie folosești altă logică
            qWarning() << "Login reply: no id field returned by server";
        }
        menu->setPlayerName(name);
        menu->show();
        this->close();
    }
    else if (statusCode == 401) {
        QMessageBox::warning(this, "Eroare", "Invalid credentials!");
    }
    else {
        QMessageBox::warning(this, "Eroare server",
            QString("HTTP %1\n\n%2").arg(statusCode).arg(QString::fromUtf8(body)));
    }

    reply->deleteLater();
}

bool LoginPage::validateInput(const QString& username, const QString& password)
{
    const QRegularExpression usernameRegex("^[A-Za-z][A-Za-z0-9]*$");
    const QRegularExpression passwordRegex("^.{8,}$");

    if (!usernameRegex.match(username).hasMatch()) {
        QMessageBox::warning(this, "Eroare",
            "Invalid Username!\nOnly digits and letters are accepted and the username should always start with a letter!");
        return false; 
    }

    if (!passwordRegex.match(password).hasMatch()) {
        QMessageBox::warning(this, "Eroare",
            "Invalid Password!\nYou need at least 8 characters (letters, digits or symbols like @!#)");
        return false; 
    }

    return true; 
}
