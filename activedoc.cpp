#include "activedoc.h"
#include "ui_activedoc.h"
#include <QDebug>
#include <QTime>
#include <QDir>
#include <QGraphicsDropShadowEffect>

ActiveDoc::ActiveDoc(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ActiveDoc)
{
    ui->setupUi(this);

    QGraphicsDropShadowEffect *sh = new QGraphicsDropShadowEffect();
    sh->setBlurRadius(8);
    sh->setOffset(2);
    sh->setColor(QColor(63, 63, 63, 180));

    idDoc    = 0;
    idWorker = 0;

    this->DBH = QSqlDatabase::addDatabase("QSQLITE","cnxnDelete"+QString::number(QDateTime::currentMSecsSinceEpoch()));
    this->DBH.setDatabaseName(QDir::homePath()+"/AppData/Roaming/bits/"+"BYASS.db");
    this->DBH.setPassword("bitProjects");
    this->DBH.setUserName("neverAsk@4Pass");
    this->DBH.open();

    ui->frame->setGraphicsEffect(sh);


    connect(ui->Bt_toArchive,SIGNAL(pressed()),this,SLOT(saveToArchive()));
    connect(ui->Bt_toArchive,SIGNAL(released()),parent,SLOT(displayArchivedDocs()));
    connect(ui->Bt_toArchive,SIGNAL(released()),this,SLOT(deleteLater()));

    connect(ui->Bt_remove,SIGNAL(pressed()),this,SLOT(cancelAssociate()));
    connect(ui->Bt_remove,SIGNAL(released()),parent,SLOT(updateLists()));
    connect(ui->Bt_remove,SIGNAL(released()),parent,SLOT(currentActiveStats()));
}

void ActiveDoc::_ActiveDoc(int idDoc,const QString &nameWorker,int idWorker,const QString &titleDoc, int totalPages, int pagesDone,QDate depositeDay,QDate deliveryDay)
{
    this->idDoc    = idDoc;
    this->idWorker = idWorker;
    ui->L_nameWorker->setText(nameWorker);
    ui->L_titleDoc->setText(" \" "+titleDoc+" \" ");

    ui->PB_advance->setMaximum(totalPages);
    ui->PB_advance->setValue(abs(pagesDone-totalPages));
    ui->PB_advance->setFormat(QString::number(pagesDone));


    QDate now = QDate::currentDate();

    ui->PB_daysLeft->setMaximum(depositeDay.daysTo(deliveryDay));
    int daysLeft = now.daysTo(deliveryDay);
    ui->PB_daysLeft->setValue(daysLeft);
    // ui->L_Cancel->setText(QString::number(daysLeft)+"Left\n"+QString::number(pagesDone)+"/"+QString::number(totalPages));
    ui->PB_daysLeft->setFormat("( "+QString::number(daysLeft)+" )"+tr("days left  "));

}

void ActiveDoc::cancelAssociate()
{
    int idDoc    = this->idDoc;
    int idWorker = this->idWorker;

    this->DBH.open();
    this->DBH.transaction();

    //! [1]
    QSqlQuery *query = new QSqlQuery(this->DBH);
    query->exec("UPDATE documents SET idWorker='-1',dateStarted='-1' WHERE id="+QString::number(idDoc)+" ");

    query->exec("UPDATE workers SET currentDocID='-1' WHERE id="+QString::number(idWorker)+" ");

    this->DBH.commit();
    // update ListDoc . but ListWorkers nothing changes.
}


void ActiveDoc::saveToArchive()
{
    this->DBH.transaction();
    QSqlQuery *query = new QSqlQuery(this->DBH);
    query->exec("UPDATE documents SET dateFinished='"+QDate::currentDate().toString("yyyy-MM-dd")+"' WHERE id="+QString::number(idDoc));
    this->DBH.commit();
    this->DBH.close();
}

ActiveDoc::~ActiveDoc()
{
    this->DBH.~QSqlDatabase();
    delete ui;
}
