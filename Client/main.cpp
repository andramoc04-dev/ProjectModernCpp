#include <QtWidgets/QApplication>
#include"MenuPage.h"
#include "GamePage.h"
#include"LoginPage.h"
#include "MinigamePage.h"
int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    LoginPage login;
    login.show();
    
    QObject::connect(&app, &QApplication::aboutToQuit, []() {
        auto* tracker = LoginPage::getPlayTimeTracker();
        if (tracker && tracker->isTracking()) {
            tracker->stopAndSync();
        }
        });
    
    return app.exec();
}
