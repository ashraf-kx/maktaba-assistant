#include "login.h"
#include "ui_login.h"

Q_LOGGING_CATEGORY(LGN,"LOGIN")

Login::Login(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Login)
{
    ui->setupUi(this);

    loginMode = false;

    ui->BT_login->setGraphicsEffect(Style::shadowbutton());
    ui->BT_cancel->setGraphicsEffect(Style::shadowbutton());

    DB = new DBH();

    idAdmin = -1;

    connect(ui->BT_login,SIGNAL(pressed()),this,SLOT(tryLogin()));
    connect(ui->BT_cancel,SIGNAL(clicked()),ui->LE_loginName,SLOT(clear()));
    connect(ui->BT_cancel,SIGNAL(clicked()),ui->LE_passwordLogin,SLOT(clear()));
}

bool Login::tryLogin()
{
    if(!ui->LE_loginName->text().isEmpty() && !ui->LE_passwordLogin->text().isEmpty())
    {
        if(DB->attemptLogin(ui->LE_loginName->text(),ui->LE_passwordLogin->text()))
        {
            loginMode = true;
            mToast = new Toast(this);
            mToast->show(tr("welcome Back."),"");
            return true;
        }
        else
        {
            mToast = new Toast(this);
            mToast->show(tr("username or password not valid."),"");
            loginMode = false;
            return false;
        }
    }
    mToast = new Toast(this);
    mToast->show(tr("fill both name and password fields."),"");
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
