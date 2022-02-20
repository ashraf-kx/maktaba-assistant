#include "tabwidgetsettings.h"
#include "ui_tabwidgetsettings.h"

Q_LOGGING_CATEGORY(STG,"SETTING")

TabWidgetSettings::TabWidgetSettings(QWidget *parent) :
    QTabWidget(parent),
    ui(new Ui::TabWidgetSettings)
{
    ui->setupUi(this);

    DB = new DBH();

    connect(ui->BT_saveAdminNameX,SIGNAL(clicked()),this,SLOT(updateAdminName()));
    connect(ui->BT_cancelAdminnX,SIGNAL(clicked()),ui->LE_nameAdmin,SLOT(clear()));

    connect(ui->BT_saveAddress,SIGNAL(clicked()),this,SLOT(updateAdminAddress()));
    connect(ui->BT_cancelAddress,SIGNAL(clicked()),ui->LE_addressAdmin,SLOT(clear()));

    connect(ui->BT_savePhone,SIGNAL(clicked()),this,SLOT(updateAdminPhone()));
    connect(ui->BT_cancelPhone,SIGNAL(clicked()),ui->LE_phoneAdmin,SLOT(clear()));

    connect(ui->BT_savePass,SIGNAL(clicked()),this,SLOT(updatePassword()));
    connect(ui->BT_cancelPass,SIGNAL(clicked()),ui->LE_newPass,SLOT(clear()));
    connect(ui->BT_cancelPass,SIGNAL(clicked()),ui->LE_repNewPass,SLOT(clear()));

    this->setVisible(true);

}

void TabWidgetSettings::updateAdminName()
{
    if(!ui->LE_nameAdmin->text().isEmpty())
    {
        if(ui->LE_nameAdmin->text().length()>6)
        {
            DB->updateAdminName(ui->LE_nameAdmin->text());

            ui->LE_nameAdmin->clear();
            mToast = new Toast(this);
            mToast->show(tr("update username DONE"),"");
        }else{
            mToast = new Toast(this);
            mToast->show(tr("username must has more then 6 caracteres"),"");
        }
    }else{
        mToast = new Toast(this);
        mToast->show(tr("write the username in the box"),"");
    }
}

void TabWidgetSettings::updatePassword()
{
    if(!ui->LE_newPass->text().isEmpty() && !ui->LE_repNewPass->text().isEmpty())
    {
        if(ui->LE_newPass->text().length()>7)
        {
            if(ui->LE_newPass->text() == ui->LE_repNewPass->text())
            {
                DB->updateAdminPassword(ui->LE_newPass->text());

                ui->LE_newPass->clear();
                ui->LE_repNewPass->clear();

                mToast = new Toast(this);
                mToast->show(tr("password updated"),"");
            }else
            {
                mToast = new Toast(this);
                mToast->show(tr("passwords does not match"),"");
            }
        }else
        {
            mToast = new Toast(this);
            mToast->show(tr("password shorter then 7 char"),"");
        }
    }else
    {
        mToast = new Toast(this);
        mToast->show(tr("password fields empty"),"");
    }
}

void TabWidgetSettings::updateAdminAddress()
{
    if(!ui->LE_addressAdmin->text().isEmpty())
    {
        if(ui->LE_addressAdmin->text().length()>6)
        {
            DB->updateAdminAddress(ui->LE_addressAdmin->text());

            ui->LE_addressAdmin->clear();
            mToast = new Toast(this);
            mToast->show(tr("update Address DONE"),"");
        }else{
            mToast = new Toast(this);
            mToast->show(tr("Address must has more then 6 caracteres"),"");
        }
    }else{
        mToast = new Toast(this);
        mToast->show(tr("write the Address in the box"),"");
    }
}

void TabWidgetSettings::updateAdminPhone()
{
    if(!ui->LE_phoneAdmin->text().isEmpty())
    {
        if(ui->LE_phoneAdmin->text().length()>9)
        {
            DB->updateAdminPhone(ui->LE_phoneAdmin->text());

            ui->LE_phoneAdmin->clear();
            mToast = new Toast(this);
            mToast->show(tr("update phone number DONE"),"");
        }else{
            mToast = new Toast(this);
            mToast->show(tr("phone number must has more then 9 numbers"),"");
        }
    }else{
        mToast = new Toast(this);
        mToast->show(tr("write the phone number in the box"),"");
    }
}

TabWidgetSettings::~TabWidgetSettings()
{
    delete ui;
}
