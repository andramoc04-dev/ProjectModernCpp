#pragma once
#include <QDialog>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QShowEvent>
#include "ui_EndgamePage.h"

class EndgamePage : public QDialog
{
    Q_OBJECT
public:
    EndgamePage(int playerId, const QString& playerName, int gameId,
        QString gameEnding, int unusedCardsCount,
        const QString& serverUrl, bool hasScoreLossProtection = false,
        QWidget* parent = nullptr);
    ~EndgamePage();

protected:
    void showEvent(QShowEvent* event) override;

private slots:
    void OnWinReplyFinished();
    void OnLossReplyFinished();
    void OnRecalculateScoreFinished();
    void OnBackToMenuClicked();

private:
    void SubmitGameResult();
    void RecordWin();
    void RecordLoss();
    void RecalculateScore();

private:
    Ui::EndgamePageClass ui;
    QNetworkAccessManager* m_network;

    int m_playerId;
    QString m_playerName;
    int m_gameId;
    QString m_gameEnding; // "won" sau "lost"
    int m_unusedCardsCount;
    QString m_serverUrl;

    bool m_resultSubmitted{ false };
    bool m_scoreRecalculated{ false };
    bool m_hasScoreLossProtection{ false };
};