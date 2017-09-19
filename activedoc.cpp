#include "activedoc.h"

#include <QDebug>
#include <QTime>
#include <QDir>

Q_LOGGING_CATEGORY(ACDC,"ACTIVEDOCS")

ActiveDoc::ActiveDoc(QWidget *parent) :
    QWidget(parent)
{
    setupUi(this);

    idDoc    = 0;
    idWorker = 0;

    DB = new DBH("_ACTIVEDOC_");

    frame->setGraphicsEffect(Style::shadow());

    connect(Bt_toArchive,SIGNAL(pressed()),this,SLOT(saveToArchive()));
    connect(Bt_toArchive,SIGNAL(released()),parent,SLOT(displayArchivedDocs()));
    connect(Bt_toArchive,SIGNAL(released()),this,SLOT(deleteLater()));

    connect(Bt_remove,SIGNAL(pressed()),this,SLOT(cancelAssociate()));
    connect(Bt_remove,SIGNAL(released()),parent,SLOT(updateLists()));
    connect(Bt_remove,SIGNAL(released()),parent,SLOT(currentActiveStats()));
}

void ActiveDoc::_ActiveDoc(int idDoc,const QString &nameWorker,int idWorker,
                           const QString &titleDoc, int totalPages,
                           int pagesDone,QDate depositeDay,QDate deliveryDay)
{
    this->idDoc    = idDoc;
    this->idWorker = idWorker;
    L_nameWorker->setText(nameWorker);

    L_titleDoc->setText(" \" "+titleDoc+" \" ");

    PB_advance->setMaximum(totalPages);
    PB_advance->setValue(abs(pagesDone-totalPages));
    PB_advance->setFormat(tr("Pages Done ")+"( "+QString::number(pagesDone)+" ) ");

    QDate now = QDate::currentDate();

    PB_daysLeft->setMaximum(depositeDay.daysTo(deliveryDay));
    int daysLeft = now.daysTo(deliveryDay);
    PB_daysLeft->setValue(daysLeft);
    PB_daysLeft->setFormat(tr("Days Left  ")+"( "+QString::number(daysLeft)+" )");
}

void ActiveDoc::cancelAssociate()
{    
    DB->cancelAssignment(this->idDoc,this->idWorker);
//    // update ListDoc . but ListWorkers nothing changes.
}

void ActiveDoc::saveToArchive()
{
    DB->moveToArchive(idDoc);
}

ActiveDoc::~ActiveDoc()
{

}
