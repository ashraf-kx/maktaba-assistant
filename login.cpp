#include "login.h"
#include "ui_login.h"
#include <QDir>
#include <QDebug>
#include <QMessageBox>
#include <QGraphicsDropShadowEffect>

Login::Login(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Login)
{
    ui->setupUi(this);

    loginMode = false;

    QGraphicsDropShadowEffect *sh = new QGraphicsDropShadowEffect();
    sh->setBlurRadius(8);
    sh->setOffset(2);
    sh->setColor(QColor(63, 63, 63, 180));
    ui->BT_login->setGraphicsEffect(sh);

    QGraphicsDropShadowEffect *sh1 = new QGraphicsDropShadowEffect();
    sh1->setBlurRadius(8);
    sh1->setOffset(2);
    sh1->setColor(QColor(63, 63, 63, 180));
    ui->BT_cancel->setGraphicsEffect(sh1);


    this->DBH = QSqlDatabase::addDatabase("QSQLITE","cnxnLogin");
    this->DBH.setDatabaseName(QDir::homePath()+"/AppData/Roaming/bits/"+"BYASS.db");
    this->DBH.setPassword("bitProjects");
    this->DBH.setUserName("neverAsk@4Pass");
    this->DBH.open();

    if(!this->DBH.isDriverAvailable("QSQLITE")){
        QMessageBox::information(this,"SQlite 3","SQLite Driver is Not Available");
    }

    idAdmin = -1;

    connect(ui->BT_login,SIGNAL(pressed()),this,SLOT(tryLogin()));
    connect(ui->BT_cancel,SIGNAL(clicked()),ui->LE_loginName,SLOT(clear()));
    connect(ui->BT_cancel,SIGNAL(clicked()),ui->LE_passwordLogin,SLOT(clear()));
}

bool Login::tryLogin()
{
    if(!ui->LE_loginName->text().isEmpty() && !ui->LE_passwordLogin->text().isEmpty())
    {
        QString username = "";
        QString password = "";

        this->DBH.transaction();
        SimpleCrypt crypto(Q_UINT64_C(0x16af28db99bbca1f));

        QSqlQuery *query = new QSqlQuery(this->DBH);
        query->prepare("SELECT username,password FROM admin");
        query->exec();

        while (query->next())
        {
            username  =  crypto.decryptToString(query->value(0).toString());
            password  =  crypto.decryptToString(query->value(1).toString());
        }
        this->DBH.commit();
        if(username == ui->LE_loginName->text() && password == ui->LE_passwordLogin->text())
        {
            loginMode = true;
            mToast = new Toast(this);
            mToast->setMessage(tr("welcome Back."));
            return true;
        }
        else
        {
            mToast = new Toast(this);
            mToast->setMessage(tr("username or password not valid."));
            loginMode = false;
            return false;
        }
    }
    mToast = new Toast(this);
    mToast->setMessage(tr("fill both name and password fields."));
    return false;
}

bool Login::getModelogin()
{
    return loginMode;
}

void Login::setModeLogin(bool val)
{
    loginMode = val;
}

QPushButton* Login::getBtLogin()
{
    return ui->BT_login;
}

Login::~Login()
{
    delete ui;
}
