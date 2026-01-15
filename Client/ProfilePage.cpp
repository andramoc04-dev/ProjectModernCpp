#include "ProfilePage.h"
#include "MenuPage.h"


ProfilePage::ProfilePage(int playerId, const QString& serverUrl, QWidget* parent)
    : QDialog(parent), m_playerId(playerId), m_serverUrl(serverUrl), network(new QNetworkAccessManager(this))
{
    ui.setupUi(this);

    QUrl url(QString("%1/player/%2").arg(m_serverUrl).arg(m_playerId));
    QNetworkRequest req(url);
    QNetworkReply* reply = network->get(req);
    connect(ui.BackButton, &QPushButton::clicked, this, &ProfilePage::onBackButtonClicked);
    connect(reply, &QNetworkReply::finished, this, &ProfilePage::onProfileReplyFinished);
}

ProfilePage::ProfilePage(int playerId, const QString& playerName, const QString& serverUrl, QWidget* parent)
    : QDialog(parent), m_playerId(playerId), m_playerName(playerName), m_serverUrl(serverUrl), network(new QNetworkAccessManager(this))
{
    ui.setupUi(this);
    QUrl url(QString("%1/player/%2").arg(m_serverUrl).arg(m_playerId));
    QNetworkRequest req(url);
    QNetworkReply* reply = network->get(req);
    connect(ui.BackButton, &QPushButton::clicked, this, &ProfilePage::onBackButtonClicked);
    connect(reply, &QNetworkReply::finished, this, &ProfilePage::onProfileReplyFinished);
}


void ProfilePage::onProfileReplyFinished()
{
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    if (!reply) return;

    if (reply->error() == QNetworkReply::NoError) {
        QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());
        if (doc.isObject()) {
            QJsonObject obj = doc.object();
            ui.usernameLabel->setText(obj["name"].toString());
            ui.scoreLabel->setText(QString::number(obj["score"].toInt()));
            ui.winsLabel->setText(QString::number(obj["wins"].toInt()));
            ui.hoursLabel->setText(QString::number(obj["hoursPlayed"].toInt()));
            ui.unusedLabel->setText(QString::number(obj["unusedCards"].toInt()));
        }
    }
    else {
        QMessageBox::warning(this, "Error", reply->errorString());
    }
    reply->deleteLater();
}


ProfilePage::~ProfilePage()
{}

void ProfilePage::onBackButtonClicked()
{
    MenuPage* menuPage = new MenuPage(m_playerId, m_playerName, nullptr);
    menuPage->show();
    this->close();
    this->deleteLater();
}


