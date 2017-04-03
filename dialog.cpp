#include "dialog.h"
#include "ui_dialog.h"
#include <QGraphicsDropShadowEffect>

Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);

    this->setWindowModality(Qt::WindowModal);
    this->setModal(true);
    //QApplication::processEvents();

    QGraphicsDropShadowEffect * sh = new QGraphicsDropShadowEffect();
    sh->setBlurRadius(8);
    sh->setOffset(2);
    sh->setColor(QColor(63, 63, 63, 180));
    ui->frame->setGraphicsEffect(sh);

    this->setWindowFlags(Qt::CustomizeWindowHint | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);

    ui->frame->setWindowOpacity(0.99);

    connect(ui->Bt_yes,SIGNAL(clicked()),this,SLOT(accept()));
    connect(ui->Bt_no,SIGNAL(clicked()),this,SLOT(reject()));

}

void Dialog::setMessage(const QString& msg,const QString& typeM)
{
    ui->labelMessage->setText(msg);
    if(typeM == "warning")
        ui->icon->setPixmap(QPixmap(":Icons/ic_warning_amber_48pt.png"));
    if(typeM == "error")
        ui->icon->setPixmap(QPixmap(":Icons/ic_error_outline_red_2x.png"));
    if(typeM == "question")
        ui->icon->setPixmap(QPixmap(":Icons/ic_help_outline_2x.png"));
    if(typeM == "done")
        ui->icon->setPixmap(QPixmap(":Icons/ic_done_2x.png"));

}

Dialog::~Dialog()
{
    delete ui;
}
