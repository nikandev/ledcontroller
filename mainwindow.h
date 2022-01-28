#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QColorDialog>
#include <QHash>
#include <QMainWindow>
#include <QNetworkAccessManager>
#include <QRgb>
#include <QSystemTrayIcon>
#include <QTimer>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

    public:
        MainWindow(QWidget *parent = nullptr);
        ~MainWindow();
    private:
        Ui::MainWindow* ui;
        QSystemTrayIcon* _tray;
        QNetworkAccessManager* _network;
        QColorDialog* _clr;
        QTimer _clickDelay;
        QRgb _lastRgb;
        QString _ip;
        void setupCoreApplication();
        void setupInitialLedState();
        void loadSettings();
        void setupTray();
        void setupNetwork();
        void setNewValue(int r, int g, int b);
        void closeEvent(QCloseEvent *event) override;
        QString buildLink() const;
        void pickColor();
        void switchOnOff();
        void setupClickDelay();
    public slots:
        void exit();
    private slots:
        void iconActivated(QSystemTrayIcon::ActivationReason reason);
        void changeIp();
};
#endif // MAINWINDOW_H
