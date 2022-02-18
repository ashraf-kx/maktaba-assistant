#include "archiveddoc.h"
#include "ui_archiveddoc.h"
#include <QDir>
#include <QGraphicsDropShadowEffect>

ArchivedDoc::ArchivedDoc(QWidget *parent) : QWidget(parent),
                                            ui(new Ui::ArchivedDoc)
{
    ui->setupUi(this);

    QGraphicsDropShadowEffect *sho = new QGraphicsDropShadowEffect();
    sho->setBlurRadius(8);
    sho->setOffset(2);
    sho->setColor(QColor(63, 63, 63, 180));

    this->setGraphicsEffect(sho);

    connect(ui->Bt_Cancel, SIGNAL(pressed()), this, SLOT(removeFromArchive()));
    connect(ui->Bt_Cancel, SIGNAL(pressed()), parent, SLOT(updateLists()));
    connect(ui->Bt_Cancel, SIGNAL(pressed()), parent, SLOT(currentActiveStats()));

    connect(ui->Bt_Cancel, SIGNAL(released()), this, SLOT(deleteLater()));
}

ArchivedDoc::~ArchivedDoc()
{
    delete ui;
}

void ArchivedDoc::removeFromArchive()
{
    QSqlDatabase connection = QSqlDatabase::database();
    if (connection.open())
    {
        connection.transaction();

        QSqlQuery *query = new QSqlQuery(connection);
        query->exec("UPDATE documents SET dateFinished='-1' WHERE id=" + QString::number(this->idDoc));
        query->clear();

        connection.commit();
        connection.close();
        qInfo() << "Item removed from Archived documents.";
    }
}

void ArchivedDoc::showArchivedDocs(int idDoc, const QString &nameClient, const QString &titleDoc, int pagesDone)
{
    this->idDoc = idDoc;
    QString mTitleDoc(tr("Empty")),
        mIsPrinted(tr("Empty")),
        mDateFinished(tr("Empty")),
        mFullname(tr("Empty"));

    int mIdClient(0),
        mTotalPages(0),
        mPagesDone(0),
        mPrice(0), mPricePaid(0);

    QSqlDatabase connection = QSqlDatabase::database();
    if (connection.open())
    {
        qInfo() << "Displaying archived docs.";
        connection.transaction();

        QSqlQuery *query = new QSqlQuery(connection);
        query->prepare("SELECT idClient,titleDoc,totalPages,pagesDone,isPrinted,dateFinished FROM documents WHERE id='" + QString::number(idDoc) + "' ");
        query->exec();

        while (query->next())
        {
            mIdClient = query->value(0).toInt();
            mTitleDoc = query->value(1).toString();
            mTotalPages = query->value(2).toInt();
            mPagesDone = query->value(3).toInt();
            mIsPrinted = query->value(4).toString();
            mDateFinished = query->value(5).toString();
        }

        query->clear();
        query->prepare("SELECT fullname,price,pricePaid FROM clients WHERE id='" + QString::number(mIdClient) + "' ");
        query->exec();
        while (query->next())
        {
            mFullname = query->value(0).toString();
            mPrice = query->value(1).toInt();
            mPricePaid = query->value(2).toInt();
        }

        query->clear();
        connection.commit();
        connection.close();
    }

    // Apply calculations on the data colleted.
    ui->Lb_nameClient->setText(mFullname);
    ui->Lb_titleBook->setText(mTitleDoc);

    if (mPagesDone < mTotalPages)
    {
        ui->Lb_pages->setStyleSheet("QLabel{background-color:gray;}");
    }
    else
    {
        ui->Lb_pages->setStyleSheet("QLabel{background-color:#55ff00;}");
    }

    if (mIsPrinted == tr("printed"))
        ui->Lb_print->setStyleSheet("QLabel{background-color:#55ff00;}");
    else
        ui->Lb_print->setStyleSheet("QLabel{background-color:gray;}");

    if (mPrice == mPricePaid)
        ui->Lb_dollar->setStyleSheet("QLabel{background-color:#55ff00;}");
    else
        ui->Lb_dollar->setStyleSheet("QLabel{background-color:gray;}");
}
