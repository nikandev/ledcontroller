#include <QCloseEvent>
#include <QColor>
#include <QColorDialog>
#include <QIcon>
#include <QInputDialog>
#include <QMenu>
#include <QNetworkRequest>
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <QSettings>
#include <QString>
#include <QUrl>

#include "mainwindow.h"
#include "ui_mainwindow.h"

#define ClickDelay 200 // ms

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow),
      _clr(nullptr),
      _ip("0.0.0.0")
{
    ui->setupUi(this);

    setupCoreApplication();
    setupClickDelay();
    setupInitialLedState();
    loadSettings();
    setupTray();
    setupNetwork();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setupCoreApplication()
{
    QCoreApplication::setOrganizationName("nikandev");
    QCoreApplication::setOrganizationDomain("github.com/nikandev");
    QCoreApplication::setApplicationName("Led Controller");
}

void MainWindow::setupClickDelay()
{
    _clickDelay.setSingleShot(true);
    connect(&_clickDelay, &QTimer::timeout, this, &MainWindow::switchOnOff);
}

void MainWindow::setupInitialLedState()
{
    _lastRgb = qRgb(0, 0, 0);
}

void MainWindow::loadSettings()
{
    QSettings settings("settings.ini", QSettings::IniFormat);
    _ip = settings.value("ip").toString();
}

void MainWindow::setupTray()
{
    QMenu* menu = new QMenu(this);
    QAction* exit = menu->addAction("Exit");
    QAction* changeIp = menu->addAction("Change IP");

    _tray = new QSystemTrayIcon(this);
    _tray->setIcon(QIcon("://offIcon.svg"));
    _tray->show();
    _tray->setContextMenu(menu);

    connect(_tray, &QSystemTrayIcon::activated, this, &MainWindow::iconActivated);
    connect(exit, &QAction::triggered, this, &MainWindow::exit);
    connect(changeIp, &QAction::triggered, this, &MainWindow::changeIp);
}

void MainWindow::setupNetwork()
{
    _network = new QNetworkAccessManager(this);
}

void MainWindow::setNewValue(int r, int g, int b)
{
    _lastRgb = qRgb(r, g, b);

    QString link = buildLink();

    _network->get(QNetworkRequest(QUrl(link)));
}

QString MainWindow::buildLink() const
{
    static const QString pref = "http://";
    static const QString af = "/?";

    QString address = pref + _ip + af;
    QString req("r" + QString::number(qRed(_lastRgb)) +
                "g" + QString::number(qGreen(_lastRgb)) +
                "b" + QString::number(qBlue(_lastRgb)));

    QString afterslash(req + "&");

    return address + afterslash;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    event->ignore();
    hide();
}

void MainWindow::exit()
{
    setNewValue(0, 0, 0);
    qApp->exit();
}

void MainWindow::iconActivated(QSystemTrayIcon::ActivationReason reason)
{
    if (_clr)
    {
        return;
    }

    switch (reason)
    {
        case QSystemTrayIcon::Trigger:
        {
            _clickDelay.start(ClickDelay);
            break;
        }
        case QSystemTrayIcon::DoubleClick:
        {
            _clickDelay.stop();
            pickColor();
            break;
        }
        default:
        {
        }
    }
}

void MainWindow::switchOnOff()
{
    if (_lastRgb != qRgb(0, 0, 0))
    {
        _tray->setIcon(QIcon("://offIcon.svg"));
        setNewValue(0, 0, 0);
    }
    else
    {
        _tray->setIcon(QIcon("://onIcon.svg"));
        setNewValue(255, 255, 255);
    }
}

void MainWindow::pickColor()
{
    _clr = new QColorDialog();
    _clr->setCurrentColor(QColor(_lastRgb));
    _clr->exec();

    QColor result = _clr->selectedColor();

    delete _clr;
    _clr = nullptr;

    if (!result.isValid())
    {
        return;
    }

    setNewValue(result.red(), result.green(), result.blue());
}

void MainWindow::changeIp()
{
    QString text = QInputDialog::getText(this, "Change IP adress",
                                         qUtf8Printable("Current adress: " + _ip), QLineEdit::Normal);
    if (!text.isEmpty())
    {
        QRegularExpression re("^(([0-9]|[1-9][0-9]|1[0-9]{2}|2[0-4][0-9]|25[0-5])\\.)"
                              "{3}([0-9]|[1-9][0-9]|1[0-9]{2}|2[0-4][0-9]|25[0-5])$");

        QRegularExpressionMatch match = re.match(text);

        if (match.hasMatch())
        {
            QSettings settings("settings.ini", QSettings::IniFormat);
            settings.setValue("ip", text);
            _ip = text;
        }
    }
}
