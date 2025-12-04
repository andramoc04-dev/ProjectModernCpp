#include "MinigamePage.h"

MinigamePage::MinigamePage(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);
    RunMinigame();
}

MinigamePage::~MinigamePage()
{}

void MinigamePage::RunMinigame()
{
    m_counter = 0;
    QGridLayout* layout = new QGridLayout(this);

    QString style =
        "QPushButton {"
        "    border: none;"
        "    background-image: url(:/Client/CardPlacer.png);"
        "}";

    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 4; j++) {
            QPushButton* button = new QPushButton(this); 
            button->setStyleSheet(style);
            button->setFixedSize(80, 80);
            layout->addWidget(button, i, j);

            connect(button, &QPushButton::clicked, this, [this, button]() {
                if (ui.ScoreNumber) {  
                    m_counter++;
                    ui.ScoreNumber->setText(QString::number(m_counter));
                }
                button->hide();
                });
        }
    }

    setLayout(layout);
}
