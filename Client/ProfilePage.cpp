#include "ProfilePage.h"
#include "MenuPage.h"
#include <QJsonObject>
#include <QJsonDocument>
#include <QDebug>
#include <QMessageBox>

ProfilePage::ProfilePage(int playerId, const QString& serverUrl, QWidget* parent)
    : ProfilePage(playerId, "", serverUrl, parent)
{
}

ProfilePage::ProfilePage(int playerId, const QString& playerName, const QString& serverUrl, QWidget* parent)
    : QDialog(parent),
    ui(new Ui::ProfilePageClass),
    m_playerId(playerId),
    m_playerName(playerName),
    m_serverUrl(serverUrl),
    network(new QNetworkAccessManager(this))
{
    ui->setupUi(this);
    connect(ui->BackButton, &QPushButton::clicked, this, &ProfilePage::onBackButtonClicked);
}

ProfilePage::~ProfilePage()
{
}

void ProfilePage::showEvent(QShowEvent* event)
{
    QDialog::showEvent(event);
    fetchProfileData();
}

void ProfilePage::fetchProfileData()
{
    if (m_playerId < 0) {
        QMessageBox::warning(this, "Eroare", "Player ID invalid!");
        return;
    }

    const QUrl url(m_serverUrl + "/player/" + QString::number(m_playerId));
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    qDebug() << "Fetching profile data from:" << url.toString();

    QNetworkReply* reply = network->get(request);
    connect(reply, &QNetworkReply::finished, this, &ProfilePage::onProfileReplyFinished);
}

void ProfilePage::onProfileReplyFinished()
{
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    if (!reply) return;

    if (reply->error() != QNetworkReply::NoError) {
        QMessageBox::warning(this, "Eroare rețea",
            "Nu s-au putut încărca datele profilului:\n" + reply->errorString());
        reply->deleteLater();
        return;
    }

    const int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    const QByteArray body = reply->readAll();

    if (statusCode == 200) {
        QJsonParseError parseError;
        const QJsonDocument doc = QJsonDocument::fromJson(body, &parseError);

        if (parseError.error == QJsonParseError::NoError && doc.isObject()) {
            const QJsonObject data = doc.object();
            updateUIWithProfileData(data);
        }
        else {
            QMessageBox::warning(this, "Eroare",
                "Răspuns invalid de la server:\n" + parseError.errorString());
        }
    }
    else if (statusCode == 404) {
        QMessageBox::warning(this, "Eroare", "Jucătorul nu a fost găsit!");
    }
    else {
        QMessageBox::warning(this, "Eroare server",
            QString("HTTP %1\n\n%2").arg(statusCode).arg(QString::fromUtf8(body)));
    }

    reply->deleteLater();
}

void ProfilePage::updateUIWithProfileData(const QJsonObject& data)
{
    // Extrage datele din JSON
    const QString username = data["name"].toString();
    const int score = data["score"].toInt();
    const int wins = data["wins"].toInt();
    const int hoursPlayed = data["hoursPlayed"].toInt();
    const int unusedCards = data["unusedCards"].toInt();

    ui->usernameLabel->setText(username);
    ui->scoreLabel->setText(QString::number(score));
    ui->winsLabel->setText(QString::number(wins));
    ui->hoursLabel->setText(QString::number(hoursPlayed));
    ui->unusedLabel->setText(QString::number(unusedCards));

    qDebug() << "Profile data updated successfully for:" << username;
}

void ProfilePage::onBackButtonClicked()
{
    MenuPage* menuPage = new MenuPage(m_playerId, m_playerName, nullptr);
    menuPage->show();
    this->close();
    this->deleteLater();
}