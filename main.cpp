#include <QApplication>
#include <QMessageBox>
#include <QObject>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    if (!QSystemTrayIcon::isSystemTrayAvailable()) {
        QMessageBox::critical(nullptr, QObject::tr("Systray"),
                              QObject::tr("I couldn't detect any system tray "
                                          "on this system."));
        return 1;
    }

    QApplication::setQuitOnLastWindowClosed(false);

    MainWindow w;
    QObject::connect(&a, SIGNAL(aboutToQuit()), &w, SLOT(exit()));


    return a.exec();
}
