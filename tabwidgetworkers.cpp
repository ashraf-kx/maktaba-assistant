#include "tabwidgetworkers.h"
#include "ui_tabwidgetworkers.h"
#include <QGraphicsDropShadowEffect>
#include <QDir>

TabWidgetWorkers::TabWidgetWorkers(QWidget *parent) :
    QTabWidget(parent),
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
    sh1->setOffset(2,2);
    sh1->setColor(QColor(63, 63, 63, 180));

    mapper = new QDataWidgetMapper();


    this->DBH = QSqlDatabase::addDatabase("QSQLITE","cnxnWorker");
    this->DBH.setDatabaseName(QDir::homePath()+"/AppData/Roaming/bits/"+"BYASS.db");
    this->DBH.setPassword("bitProjects");
    this->DBH.setUserName("neverAsk@4Pass");
    this->DBH.open();

    modelWorker = new QSqlTableModel(this,this->DBH);
    modelWorker->setTable("workers");
    modelWorker->select();
    ui->tableViewWorkers->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableViewWorkers->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableViewWorkers->resizeColumnsToContents();
    ui->tableViewWorkers->resizeRowsToContents();
    ui->tableViewWorkers->setSortingEnabled(true);
    ui->tableViewWorkers->setSelectionMode(QAbstractItemView::SingleSelection);

    QStringList listHeader;
    listHeader<<tr("Full name");
    listHeader<<tr("Phone number");
    listHeader<<tr("Email");
    listHeader<<tr("Remarke");
    listHeader<<tr("disponibility");

    modelWorker->setHeaderData(0, Qt::Horizontal, tr("N#"));
    modelWorker->setHeaderData(1, Qt::Horizontal, listHeader.at(0));
    modelWorker->setHeaderData(2, Qt::Horizontal, listHeader.at(1));
    modelWorker->setHeaderData(3, Qt::Horizontal, listHeader.at(2));
    modelWorker->setHeaderData(4, Qt::Horizontal, listHeader.at(3));
    modelWorker->setHeaderData(5, Qt::Horizontal, listHeader.at(4));

    proxyModelWorker = new QSortFilterProxyModel(this);

    proxyModelWorker->setSourceModel(modelWorker);

    proxyModelWorker->setFilterRegExp(QRegExp("", Qt::CaseInsensitive,QRegExp::FixedString));
    proxyModelWorker->setFilterKeyColumn(1);

    ui->tableViewWorkers->setModel(proxyModelWorker);

    ui->tableViewWorkers->show();

    ui->tableViewWorkers->setColumnHidden(6,true);
    ui->tableViewWorkers->setColumnHidden(7,true);
    ui->tableViewWorkers->setColumnHidden(8,true);

    startMapper();

    ui->SB_idWorkerx->setVisible(false);


    ui->BT_cancelInfoW->setGraphicsEffect(sh);
    ui->BT_saveWorker->setGraphicsEffect(sh2);
    ui->widgetSearch->setGraphicsEffect(sh1);

    // Actions.
    connect(ui->BT_saveWorker,SIGNAL(clicked()),this,SLOT(saveDemande2DB()));
    connect(ui->BT_saveWorker,SIGNAL(clicked(bool)),ui->BT_saveWorker,SLOT(setDisabled(bool)));
    connect(ui->BT_cancelInfoW,SIGNAL(clicked()),this,SLOT(clearForm()));
    connect(ui->LE_fullnameFiltre,SIGNAL(textChanged(QString)),proxyModelWorker,SLOT(setFilterRegExp(QString)));
    connect(ui->tableViewWorkers->selectionModel(), SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),
            mapper, SLOT(setCurrentModelIndex(QModelIndex)));
    connect(ui->BT_delete,SIGNAL(clicked()),this,SLOT(deleteWorker()));

    connect(ui->BT_updateWorker,SIGNAL(clicked()),this,SLOT(updateWorker()));
}

void TabWidgetWorkers::updateWorker()
{
    proxyModelWorker->submit();
    clearFormUpdate();
    emit dataWorkersChanged();
}

TabWidgetWorkers::workerData TabWidgetWorkers::getDatafromForm()
{
    workerData mWorkerData;
    mWorkerData.fullName     = ui->LE_fullNameWorker->text();
    mWorkerData.phoneNumber   = ui->LE_phoneWorker->text();
    mWorkerData.email         = ui->LE_primeryEmailW->text();
    mWorkerData.remarke       = ui->TE_remarke->toPlainText();
    mWorkerData.date_created  = QDateTime::currentDateTime().toTime_t();
    mWorkerData.date_modified = mWorkerData.date_created;
    return mWorkerData;
}

bool TabWidgetWorkers::addWorker(TabWidgetWorkers::workerData mWorkerData)
{
    this->DBH.open();
    this->DBH.transaction();

    //! [1] Save data into workers table.
    QSqlQuery *query = new QSqlQuery(this->DBH);
    query->prepare("INSERT INTO workers(id,fullName,phoneNumber,"
                   "email ,remarke,isDisponible,date_created , date_modified) "
                   "VALUES (NULL,:fullName,:phoneNumber,"
                   ":email ,:remarke,:isDisponible,:date_created ,:date_modified)");

    query->bindValue(":fullName",mWorkerData.fullName);
    query->bindValue(":phoneNumber",mWorkerData.phoneNumber);
    query->bindValue(":email", mWorkerData.email);
    query->bindValue(":remarke", mWorkerData.remarke);
    query->bindValue(":isDisponible", "yes");
    query->bindValue(":date_created",mWorkerData.date_created);
    query->bindValue(":date_modified",mWorkerData.date_modified);

    query->exec();

    if(this->DBH.commit()){
        modelWorker->select();
        emit dataWorkersChanged();
        return true;
    }
    else {
        return false;
    }
}

bool TabWidgetWorkers::verificationSubmitedData(TabWidgetWorkers::workerData mWorkerData)
{
    bool ver = true;
    if(mWorkerData.fullName.isEmpty()) ver = false;
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

    if(verificationSubmitedData(mWorkerData))
    {
        Dialog *D = new Dialog(ui->T_addWorker);
        D->setMessage(tr("sure, save this worker"),"question");
        ui->GB_addWorker->setDisabled(true);
        if(D->exec()==1)
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

void TabWidgetWorkers::startMapper(){

    mapper->setModel(proxyModelWorker);

    mapper->addMapping(ui->SB_idWorkerx,modelWorker->fieldIndex("id"));
    mapper->addMapping(ui->LE_fullNamex, modelWorker->fieldIndex("fullName"));
    mapper->addMapping(ui->LE_phonex, modelWorker->fieldIndex("phoneNumber"));
    mapper->addMapping(ui->LE_primeryEmlx, modelWorker->fieldIndex("email"));
    mapper->addMapping(ui->TE_remarkex, modelWorker->fieldIndex("remarke"));
}

void TabWidgetWorkers::clearFormUpdate()
{
    ui->SB_idWorkerx->setValue(0);
    ui->LE_fullNamex->clear();
    ui->LE_phonex->clear();
    ui->LE_primeryEmlx->clear();
    ui->TE_remarkex->clear();
}

void TabWidgetWorkers::deleteWorker()
{
    if(ui->SB_idWorkerx->value() != 0)
    {
    Dialog *D = new Dialog(ui->T_updateWorker);
    D->setMessage(tr("sure, delete this worker"),"question");
    ui->GB_updateWorker->setDisabled(true);
    if(D->exec()==1)
    {
        int idWorker = ui->SB_idWorkerx->value();
        this->DBH.open();
        this->DBH.transaction();

        int currentDocID = 0;

        //! [1] Save data into workers table.
        QSqlQuery *query = new QSqlQuery(this->DBH);
        query->prepare("SELECT currentDocID FROM workers WHERE id='"+QString::number(idWorker)+"' ");
        query->exec();
        while (query->next()) {
                currentDocID  =  query->value(0).toInt();
            }
        query->exec("UPDATE documents SET idWorker=-1 WHERE id='"+QString::number(currentDocID)+"'"
                    "AND totalPages<>pagesDone");

        query->prepare("SELECT id FROM documents WHERE idWorker='"+QString::number(idWorker)+"' ");
        query->exec();

        if(query->size()>0)
            query->exec("UPDATE workers SET isDisponible='NO' WHERE id="+QString::number(idWorker));
        else
            query->exec("DELETE FROM workers WHERE id='"+QString::number(idWorker)+"'");

        this->DBH.commit();

        modelWorker->select();
        clearFormUpdate();
        emit dataWorkersChanged();
    }
    }else{
        Toast *mToast = new Toast(this);
        mToast->setMessage(tr("select worker you want to delete from the table"));
    }
    ui->GB_updateWorker->setEnabled(true);
}
