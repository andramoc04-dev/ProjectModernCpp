#include "Client.h"
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrl>



Client::Client(QWidget* parent)
    : QMainWindow(parent),
    manager(new QNetworkAccessManager(this))
{
    ui.setupUi(this);

    // conectăm managerul la slotul care procesează răspunsul
    connect(manager, &QNetworkAccessManager::finished,
        this, &Client::onPlayerReply);

    // conectăm butonul din UI la slotul nostru
    connect(ui.btnShowPlayer, &QPushButton::clicked,
        this, &Client::on_btnShowPlayer_clicked);
}

Client::~Client()
{
}

void Client::on_btnShowPlayer_clicked()
{
    // aici faci request către serverul Crow
    int playerId = 1; // poți lua dintr-un QLineEdit dacă vrei să fie dinamic
    QString url = QString("http://localhost:18080/player/%1").arg(playerId);


    QUrl qurl(url);
    QNetworkRequest request(qurl);

    manager->get(request);  
}

void Client::onPlayerReply(QNetworkReply* reply)
{
    if (reply->error() == QNetworkReply::NoError) {
        QByteArray responseData = reply->readAll();
        QJsonDocument doc = QJsonDocument::fromJson(responseData);
        QJsonObject obj = doc.object();

        QString name = obj["name"].toString();
        int score = obj["score"].toInt();
        int wins = obj["wins"].toInt();
        int hours = obj["hoursPlayed"].toInt();
        int unused = obj["unusedCards"].toInt();

        QMessageBox::information(this, "Profil jucător",
            QString("Nume: %1\nScor: %2\nVictorii: %3\nOre jucate: %4\nCărți nefolosite: %5")
            .arg(name).arg(score).arg(wins).arg(hours).arg(unused));
    }
    else {
        QMessageBox::warning(this, "Eroare", "Nu am putut obține datele jucătorului.");
    }
    reply->deleteLater();
}