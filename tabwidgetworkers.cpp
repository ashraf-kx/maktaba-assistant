#include "tabwidgetworkers.h"
#include "ui_tabwidgetworkers.h"
#include <QGraphicsDropShadowEffect>
#include <QDir>

TabWidgetWorkers::TabWidgetWorkers(QWidget *parent) : QTabWidget(parent),
                                                      ui(new Ui::TabWidgetWorkers)
{
    ui->setupUi(this);

    QGraphicsDropShadowEffect *sh = new QGraphicsDropShadowEffect();
    sh->setBlurRadius(8);
    sh->setOffset(2);
    sh->setColor(QColor(63, 63, 63, 180));

    QGraphicsDropShadowEffect *sh2 = new QGraphicsDropShadowEffect();
    sh2->setBlurRadius(8);
    sh2->setOffset(2);
    sh2->setColor(QColor(63, 63, 63, 180));

    QGraphicsDropShadowEffect *sh1 = new QGraphicsDropShadowEffect();
    sh1->setBlurRadius(8);
    sh1->setOffset(2, 2);
    sh1->setColor(QColor(63, 63, 63, 180));

    QGraphicsDropShadowEffect *shBA = new QGraphicsDropShadowEffect();
    shBA->setBlurRadius(8);
    shBA->setOffset(2, 2);
    shBA->setColor(QColor(63, 63, 63, 180));

    QGraphicsDropShadowEffect *shBD = new QGraphicsDropShadowEffect();
    shBD->setBlurRadius(8);
    shBD->setOffset(2, 2);
    shBD->setColor(QColor(63, 63, 63, 180));

    mapper = new QDataWidgetMapper();

    QSqlDatabase connection = QSqlDatabase::database();
    qInfo() << "workers connection is open: " << connection.open();
    modelWorker = new QSqlTableModel(this, connection);
    modelWorker->setTable("workers");
    modelWorker->select();

    ui->tableViewWorkers->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableViewWorkers->setSelectionBehavior(QAbstractItemView::SelectRows);

    ui->tableViewWorkers->setSortingEnabled(true);
    ui->tableViewWorkers->setSelectionMode(QAbstractItemView::SingleSelection);

    QStringList listHeader;
    listHeader << tr("Full name");
    listHeader << tr("Phone number");
    listHeader << tr("Email");
    listHeader << tr("Remarke");
    listHeader << tr("disponibility");

    modelWorker->setHeaderData(0, Qt::Horizontal, tr("N#"));
    modelWorker->setHeaderData(1, Qt::Horizontal, listHeader.at(0));
    modelWorker->setHeaderData(2, Qt::Horizontal, listHeader.at(1));
    modelWorker->setHeaderData(3, Qt::Horizontal, listHeader.at(2));
    modelWorker->setHeaderData(4, Qt::Horizontal, listHeader.at(3));
    modelWorker->setHeaderData(5, Qt::Horizontal, listHeader.at(4));

    proxyModelWorker = new QSortFilterProxyModel(this);

    proxyModelWorker->setSourceModel(modelWorker);

    proxyModelWorker->setFilterRegExp(QRegExp("", Qt::CaseInsensitive, QRegExp::FixedString));
    proxyModelWorker->setFilterKeyColumn(1);

    ui->tableViewWorkers->setModel(proxyModelWorker);
    ui->tableViewWorkers->horizontalHeader()->stretchLastSection();
    ui->tableViewWorkers->verticalHeader()->stretchLastSection();
    ui->tableViewWorkers->resizeColumnsToContents();
    ui->tableViewWorkers->show();

    ui->tableViewWorkers->setColumnHidden(6, true);
    ui->tableViewWorkers->setColumnHidden(7, true);
    ui->tableViewWorkers->setColumnHidden(8, true);

    startMapper();

    ui->SB_idWorkerx->setVisible(false);

    ui->BT_cancelInfoW->setGraphicsEffect(sh);
    ui->BT_saveWorker->setGraphicsEffect(sh2);
    ui->widgetSearch->setGraphicsEffect(sh1);
    ui->BT_updateWorker->setGraphicsEffect(shBA);
    ui->BT_delete->setGraphicsEffect(shBD);

    mapper->setSubmitPolicy(mapper->ManualSubmit);

    // Actions.
    connect(ui->BT_saveWorker, SIGNAL(clicked()), this, SLOT(saveDemande2DB()));
    connect(ui->BT_saveWorker, SIGNAL(clicked(bool)), ui->BT_saveWorker, SLOT(setDisabled(bool)));
    connect(ui->BT_cancelInfoW, SIGNAL(clicked()), this, SLOT(clearForm()));
    connect(ui->LE_fullnameFiltre, SIGNAL(textChanged(QString)), proxyModelWorker, SLOT(setFilterRegExp(QString)));
    connect(ui->tableViewWorkers->selectionModel(), SIGNAL(currentRowChanged(QModelIndex, QModelIndex)),
            mapper, SLOT(setCurrentModelIndex(QModelIndex)));
    connect(ui->BT_delete, SIGNAL(clicked()), this, SLOT(deleteWorker()));

    connect(ui->BT_updateWorker, SIGNAL(clicked()), this, SLOT(updateWorker()));
    connect(ui->CB_disponibility, SIGNAL(currentIndexChanged(int)),
            this, SLOT(changeIconDisponibility()));

    connect(ui->SB_idWorkerx, SIGNAL(valueChanged(QString)),
            ui->LB_displayIdWx, SLOT(setText(QString)));
}

void TabWidgetWorkers::updateWorker()
{
    int idWorker = ui->SB_idWorkerx->value();
    if (idWorker != 0)
    {
        QSqlDatabase connection = QSqlDatabase::database();
        if (connection.open())
        {
            qInfo() << "update an existing employer: " << connection.isOpen();
            connection.transaction();

            QSqlQuery *query = new QSqlQuery(connection);

            query->prepare("UPDATE workers SET "
                           "fullName=:fullName,phoneNumber=:phoneNumber,email=:email,"
                           "remarke=:remarke,isDisponible=:isDisponible WHERE "
                           "id=:id");

            query->bindValue(":id", ui->SB_idWorkerx->value());
            query->bindValue(":fullName", ui->LE_fullNamex->text());
            query->bindValue(":phoneNumber", ui->LE_phonex->text());
            query->bindValue(":email", ui->LE_primeryEmlx->text());
            query->bindValue(":remarke", ui->TE_remarkex->toPlainText());
            query->bindValue(":isDisponible", ui->CB_disponibility->currentText());

            query->exec();
            query->clear();

            connection.commit();

            modelWorker->select();
            ui->tableViewWorkers->update();

            clearFormUpdate();

            emit dataWorkersChanged();

            connection.close();
        }
    }
    else
    {
        Toast *mToast = new Toast(this);
        mToast->setMessage(tr("select Worker you want to update from the table"));
    }
}

TabWidgetWorkers::workerData TabWidgetWorkers::getDatafromForm()
{
    workerData mWorkerData;
    mWorkerData.fullName = ui->LE_fullNameWorker->text();
    mWorkerData.phoneNumber = ui->LE_phoneWorker->text();
    mWorkerData.email = ui->LE_primeryEmailW->text();
    mWorkerData.remarke = ui->TE_remarke->toPlainText();
    mWorkerData.date_created = QDateTime::currentDateTime().toTime_t();
    mWorkerData.date_modified = mWorkerData.date_created;
    return mWorkerData;
}

void TabWidgetWorkers::changeIconDisponibility()
{
    if (ui->CB_disponibility->currentIndex() == 0)
    {
        ui->LB_AvIcon->setStyleSheet("QLabel{background-color: #2196F3;}");
        ui->LB_AvIcon->setPixmap(QPixmap("://Icons/ic_done_white_24dp.png"));
    }
    else
    {
        ui->LB_AvIcon->setPixmap(QPixmap("://Icons/ic_clear_white_2x.png"));
        ui->LB_AvIcon->setStyleSheet("QLabel{background-color: #9aa1a6;}");
    }
}

bool TabWidgetWorkers::addWorker(TabWidgetWorkers::workerData mWorkerData)
{
    QSqlDatabase connection = QSqlDatabase::database();
    if (connection.open())
    {
        qInfo() << "Add new worker: " << connection.isOpen();
        connection.transaction();

        //! [1] Save data into workers table.
        QSqlQuery *query = new QSqlQuery(connection);
        query->prepare("INSERT INTO workers(id,fullName,phoneNumber,"
                       "email ,remarke,isDisponible,date_created , date_modified) "
                       "VALUES (NULL,:fullName,:phoneNumber,"
                       ":email ,:remarke,:isDisponible,:date_created ,:date_modified)");

        query->bindValue(":fullName", mWorkerData.fullName);
        query->bindValue(":phoneNumber", mWorkerData.phoneNumber);
        query->bindValue(":email", mWorkerData.email);
        query->bindValue(":remarke", mWorkerData.remarke);
        query->bindValue(":isDisponible", tr("worker disponible"));
        query->bindValue(":date_created", mWorkerData.date_created);
        query->bindValue(":date_modified", mWorkerData.date_modified);

        query->exec();

        connection.commit();

        modelWorker->select();
        ui->tableViewWorkers->update();

        emit dataWorkersChanged();

        connection.close();

        return true;
    }
    return false;
}

bool TabWidgetWorkers::verificationSubmitedData(TabWidgetWorkers::workerData mWorkerData)
{
    bool ver = true;
    if (mWorkerData.fullName.isEmpty())
        ver = false;
    return ver;
}

TabWidgetWorkers::~TabWidgetWorkers()
{
    delete ui;
}

void TabWidgetWorkers::clearForm()
{
    ui->LE_fullNameWorker->clear();
    ui->LE_phoneWorker->clear();
    ui->LE_primeryEmailW->clear();
    ui->TE_remarke->clear();
}

void TabWidgetWorkers::saveDemande2DB()
{
    workerData mWorkerData = getDatafromForm();

    if (verificationSubmitedData(mWorkerData))
    {
        Dialog *D = new Dialog(ui->T_addWorker);
        D->setMessage(tr("sure, save this worker"), "question");
        ui->GB_addWorker->setDisabled(true);
        if (D->exec() == 1)
        {
            addWorker(mWorkerData);
            clearForm();
            ui->BT_saveWorker->setEnabled(true);
        }
        ui->GB_addWorker->setEnabled(true);
    }
    else
    {
        Toast *mToast = new Toast(this);
        mToast->setMessage(tr("atleast give the workerName"));
        ui->BT_saveWorker->setEnabled(true);
    }
}

void TabWidgetWorkers::startMapper()
{

    mapper->setModel(proxyModelWorker);

    mapper->addMapping(ui->SB_idWorkerx, modelWorker->fieldIndex("id"));
    mapper->addMapping(ui->LE_fullNamex, modelWorker->fieldIndex("fullName"));
    mapper->addMapping(ui->LE_phonex, modelWorker->fieldIndex("phoneNumber"));
    mapper->addMapping(ui->LE_primeryEmlx, modelWorker->fieldIndex("email"));
    mapper->addMapping(ui->TE_remarkex, modelWorker->fieldIndex("remarke"));
    mapper->addMapping(ui->CB_disponibility, modelWorker->fieldIndex("isDisponible"));
}

void TabWidgetWorkers::clearFormUpdate()
{
    ui->LB_displayIdWx->setText("*");
    ui->SB_idWorkerx->setValue(0);
    ui->LE_fullNamex->clear();
    ui->LE_phonex->clear();
    ui->LE_primeryEmlx->clear();
    ui->TE_remarkex->clear();
    ui->CB_disponibility->setCurrentIndex(0);
}

void TabWidgetWorkers::deleteWorker()
{
    if (ui->SB_idWorkerx->value() != 0)
    {
        Dialog *D = new Dialog(ui->T_updateWorker);
        D->setMessage(tr("sure, delete this worker"), "question");
        ui->GB_updateWorker->setDisabled(true);
        if (D->exec() == 1)
        {
            int idWorker = ui->SB_idWorkerx->value();

            QSqlDatabase connection = QSqlDatabase::database();
            if (connection.open())
            {
                qInfo() << "Delete a worker: " << connection.isOpen();
                connection.transaction();

                int currentDocID = 0;

                //! [1] Save data into workers table.
                QSqlQuery *query = new QSqlQuery(connection);
                query->prepare("SELECT currentDocID FROM workers WHERE id='" + QString::number(idWorker) + "' ");
                query->exec();

                while (query->next())
                    currentDocID = query->value(0).toInt();

                query->clear();
                query->exec("UPDATE documents SET idWorker=-1 WHERE id='" + QString::number(currentDocID) + "'"
                                                                                                            "AND totalPages<>pagesDone");

                query->clear();
                query->prepare("SELECT id FROM documents WHERE idWorker='" + QString::number(idWorker) + "' ");
                query->exec();

                (query->size() > 0)
                    ? query->exec("UPDATE workers SET isDisponible='NO' WHERE id=" + QString::number(idWorker))
                    : query->exec("DELETE FROM workers WHERE id='" + QString::number(idWorker) + "'");

                query->clear();
                connection.commit();

                modelWorker->select();
                ui->tableViewWorkers->update();

                clearFormUpdate();
                emit dataWorkersChanged();

                connection.close();
            }
        }
    }
    else
    {
        Toast *mToast = new Toast(this);
        mToast->setMessage(tr("select worker you want to delete from the table"));
    }
    ui->GB_updateWorker->setEnabled(true);
}
