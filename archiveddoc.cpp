#include "archiveddoc.h"
#include "ui_archiveddoc.h"
#include <QDir>
#include <QGraphicsDropShadowEffect>

ArchivedDoc::ArchivedDoc(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ArchivedDoc)
{
    ui->setupUi(this);

    QGraphicsDropShadowEffect *sho = new QGraphicsDropShadowEffect();
    sho->setBlurRadius(8);
    sho->setOffset(2);
    sho->setColor(QColor(63, 63, 63, 180));

    this->DBH = QSqlDatabase::addDatabase("QSQLITE","cnxnDeleteArch"+QString::number(QTime::currentTime().msec()));
    this->DBH.setDatabaseName(QDir::homePath()+"/AppData/Roaming/bits/"+"BYASS.db");
    this->DBH.setPassword("bitProjects");
    this->DBH.setUserName("neverAsk@4Pass");
    this->DBH.open();


    this->setGraphicsEffect(sho);

    connect(ui->Bt_Cancel,SIGNAL(pressed()),this,SLOT(removeFromArchive()));
    connect(ui->Bt_Cancel,SIGNAL(pressed()),parent,SLOT(updateLists()));
    connect(ui->Bt_Cancel,SIGNAL(pressed()),parent,SLOT(currentActiveStats()));

    connect(ui->Bt_Cancel,SIGNAL(released()),this,SLOT(deleteLater()));
}

ArchivedDoc::~ArchivedDoc()
{
    delete ui;
}

void ArchivedDoc::removeFromArchive()
{
    this->DBH.transaction();
    QSqlQuery *query = new QSqlQuery(this->DBH);
    query->exec("UPDATE documents SET dateFinished='-1' WHERE id="+QString::number(this->idDoc));
    this->DBH.commit();
}

void ArchivedDoc::showArchivedDocs(int idDoc, const QString &nameClient, const QString &titleDoc, int pagesDone)
{
    this->DBH.transaction();
    //
    this->idDoc = idDoc;
    ui->L_showMessage->setText(nameClient+":\n \" "+titleDoc+"\" "+tr(" pages ")+QString::number(pagesDone)+" )");
    this->DBH.commit();
}
