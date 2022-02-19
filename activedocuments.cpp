#include "activedocuments.h"
#include "ui_activedocuments.h"
#include <QDebug>
#include <QTime>
#include <QDir>
#include <QGraphicsDropShadowEffect>

ActiveDocuments::ActiveDocuments(QWidget *parent) : QWidget(parent),
                                        ui(new Ui::ActiveDoc)
{
    ui->setupUi(this);

    QGraphicsDropShadowEffect *sh = new QGraphicsDropShadowEffect();
    sh->setBlurRadius(8);
    sh->setOffset(2);
    sh->setColor(QColor(63, 63, 63, 180));

    idDocument = 0;
    idWorker = 0;

    ui->frame->setGraphicsEffect(sh);

    connect(ui->Bt_toArchive, SIGNAL(pressed()), this, SLOT(saveToArchive()));
    connect(ui->Bt_toArchive, SIGNAL(released()), parent, SLOT(displayArchivedDocs()));
    connect(ui->Bt_toArchive, SIGNAL(released()), this, SLOT(deleteLater()));

    connect(ui->Bt_remove, SIGNAL(pressed()), this, SLOT(cancelAssociate()));
    connect(ui->Bt_remove, SIGNAL(released()), parent, SLOT(updateLists()));
    connect(ui->Bt_remove, SIGNAL(released()), parent, SLOT(currentActiveStats()));
}

void ActiveDocuments::_ActiveDoc(int idDoc, const QString &nameWorker, int idWorker, const QString &titleDoc, int totalPages, int pagesDone, QDate depositeDay, QDate deliveryDay)
{
    this->idDocument = idDoc;
    this->idWorker = idWorker;
    ui->L_nameWorker->setText(nameWorker);
    ui->L_titleDoc->setText(" \" " + titleDoc + " \" ");

    ui->PB_advance->setMaximum(totalPages);
    ui->PB_advance->setValue(abs(pagesDone - totalPages));
    ui->PB_advance->setFormat(tr("Pages Done ") + "( " + QString::number(pagesDone) + " ) ");

    QDate now = QDate::currentDate();

    ui->PB_daysLeft->setMaximum(depositeDay.daysTo(deliveryDay));
    int daysLeft = now.daysTo(deliveryDay);
    ui->PB_daysLeft->setValue(daysLeft);
    ui->PB_daysLeft->setFormat(tr("days left  ") + "( " + QString::number(daysLeft) + " )");
}

void ActiveDocuments::cancelAssociate()
{
    int idDoc = this->idDocument;
    int idWorker = this->idWorker;

    QSqlDatabase connection = QSqlDatabase::database();
    if (connection.open())
    {
        connection.transaction();

        QSqlQuery *query = new QSqlQuery(connection);
        query->exec("UPDATE documents SET idWorker=-1,dateStarted=-1 WHERE id=" + QString::number(idDoc) + " ");
        query->clear();

        query->exec("UPDATE workers SET currentDocID=-1 WHERE id=" + QString::number(idWorker) + " ");
        query->clear();

        connection.commit();
        connection.close();
    }
}

void ActiveDocuments::saveToArchive()
{
    QSqlDatabase connection = QSqlDatabase::database();
    if (connection.open())
    {
        connection.transaction();

        QSqlQuery *query = new QSqlQuery(connection);
        query->exec("UPDATE documents SET dateFinished='" + QDate::currentDate().toString("yyyy-MM-dd") + "' WHERE id=" + QString::number(idDocument));
        query->clear();

        connection.commit();
        connection.close();
    }
}

ActiveDocuments::~ActiveDocuments()
{
    delete ui;
}
