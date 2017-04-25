#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "login.h"
#include "dialog.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    this->showMaximized();
    this->setWindowTitle(tr("Librery helper"));
    this->setWindowIcon(QIcon(":Icons/icon-evergreen-alt.ico"));

    shadowWidget= new QGraphicsDropShadowEffect();
    shadowWidget->setBlurRadius(8);
    shadowWidget->setXOffset(0);
    shadowWidget->setOffset(2);
    shadowWidget->setColor(QColor(63, 63, 63, 180));

    systemTray = new QSystemTrayIcon(this);
    configAction = new QAction(tr("Settings"), this);
    configAction->setIconVisibleInMenu(true);
    configAction->setIcon(QIcon(":Icons/ic_settings_2x.png"));
    configAction->setDisabled(true);

    connect(configAction,SIGNAL(triggered()),this,SLOT(displaySetting()));

    maximizeAction = new QAction(tr("Maximize"), this);
    maximizeAction->setIconVisibleInMenu(true);
    maximizeAction->setIcon(QIcon(":Icons/ic_zoom_out_map_2x.png"));

    connect(maximizeAction, SIGNAL(triggered()), this, SLOT(showMaximized()));

    logoutAction = new QAction(tr("Log out"), this);
    logoutAction->setIconVisibleInMenu(true);
    logoutAction->setIcon(QIcon(":Icons/ic_exit_to_app_2x.png"));
    logoutAction->setDisabled(true);
    logoutAction->setVisible(false);

    connect(logoutAction, SIGNAL(triggered()), this, SLOT(changeModeLogin()));

    quitAction = new QAction(tr("Quit"), this);
    quitAction->setIconVisibleInMenu(true);
    quitAction->setIcon(QIcon(":Icons/ic_power_settings_new_2x.png"));
    quitAction->setVisible(false);

    connect(quitAction, SIGNAL(triggered()), this, SLOT(close()));

    trayMenu = new QMenu(this);
    trayMenu->addAction(configAction);
    trayMenu->addAction(maximizeAction);
    trayMenu->addAction(logoutAction);
    trayMenu->addSeparator();
    trayMenu->addAction(quitAction);

    systemTray->setContextMenu(trayMenu);
    systemTray->setIcon(QIcon(QPixmap(":Icons/icon-evergreen-alt.ico")));
    systemTray->show();

    systemTray->showMessage(tr("Librery YS"),tr("Develop everywhere."),QSystemTrayIcon::Information,1000);

    mDB = new DB();

    mTopNav            = new topNav(this);
    mTabWidgetClient   = new TabWidgetClients(this);
    mTabWidgetWorker   = new TabWidgetWorkers(this);
    mTabWidgetDocument = new TabWidgetDocuments(this);
    mLogin             = new Login();

    mTopNav->setGraphicsEffect(shadowWidget);

    ui->centralWidget->layout()->addWidget(mLogin);
    mLogin->setVisible(true);

    connect(mTopNav->getBTClient(),SIGNAL(clicked()),this,SLOT(showTClient()));
    connect(mTopNav->getBTWorker(),SIGNAL(clicked()),this,SLOT(showTWorker()));
    connect(mTopNav->getBTDocs(),SIGNAL(clicked()),this,SLOT(showTDocument()));

    connect(mTopNav->getBTLogOut(),SIGNAL(pressed()),this,SLOT(changeModeLogin()));
   
    connect(mLogin->getBtLogin(),SIGNAL(released()),this,SLOT(switchMode()));

    connect(mTabWidgetClient,SIGNAL(dataClientsChanged()),
            mTabWidgetDocument,SLOT(globaleUpdate()));

    connect(mTabWidgetWorker,SIGNAL(dataWorkersChanged()),
            mTabWidgetDocument,SLOT(globaleUpdate()));
}

MainWindow::~MainWindow()
{
    delete mTabWidgetSettings;
    delete ui;
}

void MainWindow::changeModeLogin()
{

    Dialog *mDialog = new Dialog(this);
    mDialog->setMessage(tr("sure, logout ?"),"question");
    this->centralWidget()->setDisabled(true);
    logoutAction->setDisabled(true);
    if(mDialog->exec()==1){

        mLogin->setModeLogin(false);
        switchMode();
    }
    logoutAction->setEnabled(true);
    this->centralWidget()->setEnabled(true);

}

void MainWindow::switchMode()
{
    if(mLogin->getModelogin())
    {
        ui->centralWidget->layout()->removeWidget(mLogin);
        mLogin->setVisible(false);
        configAction->setDisabled(false);
        logoutAction->setDisabled(false);

        ui->centralWidget->layout()->addWidget(mTopNav);
        mTopNav->setVisible(true);
        mTopNav->changeStyleClient();
        ui->centralWidget->layout()->addWidget(mTabWidgetClient);
        mTabWidgetClient->setVisible(true);
    }
    else
    {
        ui->centralWidget->layout()->addWidget(mLogin);
        mLogin->setVisible(true);
        configAction->setDisabled(true);
        logoutAction->setDisabled(true);

        ui->centralWidget->layout()->removeWidget(mTopNav);
        mTopNav->setVisible(false);
        ui->centralWidget->layout()->removeWidget(mTabWidgetClient);
        mTabWidgetClient->setVisible(false);
        ui->centralWidget->layout()->removeWidget(mTabWidgetWorker);
        mTabWidgetWorker->setVisible(false);
        ui->centralWidget->layout()->removeWidget(mTabWidgetDocument);
        mTabWidgetDocument->setVisible(false);
    }
}

void MainWindow::showTClient()
{
    displayMessage(tr("show client"));
    ui->centralWidget->layout()->removeWidget(mTabWidgetWorker);
    mTabWidgetWorker->setVisible(false);
    ui->centralWidget->layout()->removeWidget(mTabWidgetDocument);
    mTabWidgetDocument->setVisible(false);


    ui->centralWidget->layout()->addWidget(mTabWidgetClient);
    mTabWidgetClient->setVisible(true);
}

void MainWindow::showTWorker()
{
    displayMessage(tr("show Worker"));
    ui->centralWidget->layout()->removeWidget(mTabWidgetClient);
    mTabWidgetClient->setVisible(false);
    ui->centralWidget->layout()->removeWidget(mTabWidgetDocument);
    mTabWidgetDocument->setVisible(false);


    ui->centralWidget->layout()->addWidget(mTabWidgetWorker);
    mTabWidgetWorker->setVisible(true);
}

void MainWindow::showTDocument()
{
    displayMessage(tr("show documents"));
    ui->centralWidget->layout()->removeWidget(mTabWidgetClient);
    mTabWidgetClient->setVisible(false);
    ui->centralWidget->layout()->removeWidget(mTabWidgetWorker);
    mTabWidgetWorker->setVisible(false);


    ui->centralWidget->layout()->addWidget(mTabWidgetDocument);
    mTabWidgetDocument->setVisible(true);

}

void MainWindow::displaySetting()
{
    mTabWidgetSettings = new TabWidgetSettings();
    mTabWidgetSettings->setVisible(true);
}

void MainWindow::displayMessage(const QString &msg)
{
    ui->statusBar->showMessage(msg,10000);
}
