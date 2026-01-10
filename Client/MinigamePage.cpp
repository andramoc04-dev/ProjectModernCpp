#include "MinigamePage.h"
#include "GamePage.h"
#include <QCloseEvent>

MinigamePage::MinigamePage(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);
    RunMinigame();
}

MinigamePage::~MinigamePage()
{}

bool MinigamePage::event(QEvent* e)
{
    if (e->type() == QEvent::User) {
        updateTimerUi();
        if (m_gameTimer.running() && !m_gameTimer.expired()) {
            QCoreApplication::postEvent(this, new QEvent(QEvent::User));
        }
        return true;
    }

    return QDialog::event(e);
}

void MinigamePage::updateTimerUi()
{
    if (!ui.m_timerLabel) return;

    const int elapsed = m_gameTimer.elapsedSeconds();
    const int remaining = std::max(0, m_totalSeconds - elapsed);
    ui.m_timerLabel->setText(QString("Time: %1").arg(remaining));

    if (m_gameTimer.expired() && !m_finishedEmitted) {
        m_finishedEmitted = true;

        emit minigameFinished(
            property("playerId").toInt(),
            property("playerName").toString(),
            property("gameId").toInt(),
            property("opponentId").toInt()
        );

        close();
    }

}

void MinigamePage::RunMinigame()
{
    m_counter = 0;

    QGridLayout* layout = new QGridLayout(this);

    layout->addWidget(ui.m_timerLabel, 0, 0, 1, 2);
    layout->addWidget(ui.Score, 0, 2, 1, 1);
    layout->addWidget(ui.ScoreNumber, 0, 3, 1, 1);

    ui.m_timerLabel->setText("Time: 10");
    ui.ScoreNumber->setText("0");

    const QString style =
        "QPushButton {"
        "    border: none;"
        "    background-image: url(:/Client/resources/CardPlacer.png);"
        "}";

    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 4; j++) {
            QPushButton* button = new QPushButton(this);
            button->setStyleSheet(style);
            button->setFixedSize(80, 80);
            layout->addWidget(button, i + 1, j);

            connect(button, &QPushButton::clicked, this, [this, button]() {
                m_counter++;
                ui.ScoreNumber->setText(QString::number(m_counter));
                button->hide();
                });
        }
    }

    setLayout(layout);

    m_totalSeconds = 10;
    m_gameTimer.start(m_totalSeconds);
    QCoreApplication::postEvent(this, new QEvent(QEvent::User));
}

void MinigamePage::closeEvent(QCloseEvent* event)
{

    event->accept();
}

