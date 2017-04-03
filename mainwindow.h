#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSystemTrayIcon>
#include <QMenu>
#include "tabwidgetclients.h"
#include "tabwidgetworkers.h"
#include "tabwidgetdocuments.h"
#include "tabwidgetsettings.h"
#include "topnav.h"
#include "login.h"
#include "db.h"
#include <QGraphicsDropShadowEffect>
#include "toast.h"


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void showTClient();
    void showTWorker();
    void showTDocument();
    void switchMode();
    void changeModeLogin();
    void displayMessage(const QString &msg);
    void displaySetting();

private:
    Ui::MainWindow *ui;
    DB                 *mDB;
    TabWidgetClients   *mTabWidgetClient;
    TabWidgetWorkers   *mTabWidgetWorker;
    TabWidgetDocuments *mTabWidgetDocument;
    TabWidgetSettings  *mTabWidgetSettings;
    Login              *mLogin;
    topNav             *mTopNav;

    QSystemTrayIcon    *systemTray;
    QMenu              *trayMenu;

    QAction *configAction;
    QAction *maximizeAction;
    QAction *logoutAction;
    QAction *quitAction;

    QGraphicsDropShadowEffect *shadowBtn;
    QGraphicsDropShadowEffect *shadowLbl;
    QGraphicsDropShadowEffect *shadowLineE;
    QGraphicsDropShadowEffect *shadowWidget;
};

#endif // MAINWINDOW_H
