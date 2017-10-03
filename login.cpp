#include "login.h"
#include "ui_login.h"

#include <QWebEngineView>
#include <QQuickWidget>

Q_LOGGING_CATEGORY(LGN,"LOGIN")

Login::Login(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Login)
{
    ui->setupUi(this);

    loginMode = false;

    ui->BT_login->setGraphicsEffect(Style::shadowbutton());
    ui->BT_cancel->setGraphicsEffect(Style::shadowbutton());

    DB = new DBH("_login_");

    qCDebug(LGN)<<"Connection DB Login "<<DB->isOpen();

    idAdmin = -1;

    QWebEngineView *view = new QWebEngineView();
//    view->load(QUrl("https://www.facebook.com/v2.10/dialog/oauth?"
//                    "client_id=902456726583634"
//                    "&redirect_uri=https://www.facebook.com/connect/login_success.html"
//                    "&response_type=token"));

    view->load(QUrl("http://googleads.g.doubleclick.net/pcs/click?xai=AKAOjsuOaD3XTcrh9gS9mdjqn9zYqJ4yE_dV3iqPc-1rM2bd-EfhuPvW_PYVlTbKpHq8Azphvb_jFpB0gk9UZCMu5IifhP5mfjxlnjsFfhCTUsc4rLJLYjhAo1EP6zzsmtNw-UuXhJL006cJlHaKtR3hChjBhZc09h9SNy4oL4JUtub8_3Afj8nuERwlntx4sUVWKRjRMQUTvok9cpgIebAxtqX4XDK0kORbVc4KHcSjG5w8kZqtzoKhsl8cZJUc_esBdo2jvA9xMuBtJg&sig=Cg0ArKJSzCm67IxDFb5g&adurl=http://stackoverflow.com:80/jobs%3Futm_source%3Dwebsite%26utm_medium%3Dbanner%26utm_content%3Dleaderboard_1%26utm_campaign%3Dhouse_ads_house_ads_ROS_SO&nm=4&nx=289&ny=-28&mb=2"));
    ui->mainlayout->addWidget(view);
    view->setMaximumSize(500,400);
    view->page();
    view->show();

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
