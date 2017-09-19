#include "tabwidgetworkers.h"
#include "ui_tabwidgetworkers.h"
#include <QGraphicsDropShadowEffect>
#include <QDir>

Q_LOGGING_CATEGORY(WRK,"WORKER")

TabWidgetWorkers::TabWidgetWorkers(QWidget *parent) :
    QTabWidget(parent),
    ui(new Ui::TabWidgetWorkers)
{
    ui->setupUi(this);

    mapper = new QDataWidgetMapper();

    DB = new DBH("_workers_");

    queryModelWorker = new QSqlQueryModel();
    queryModelWorker->setQuery("SELECT `id`,`fullName`,`phoneNumber`,"
                               "`email`,`remarke`,`isDisponible`,"
                               "`date_created` FROM `workers` ",DB->getCnx());

    TA.clear();
    TA["id"]          = 0;
    TA["fullname"]    = 1;
    TA["phoneNumber"] = 2;
    TA["email"]       = 3;
    TA["remarke"]     = 4;
    TA["isDisponible"]= 5;
    TA["date_created"]= 6;

    queryModelWorker->setHeaderData(TA["id"], Qt::Horizontal, tr("N#"));
    queryModelWorker->setHeaderData(TA["fullname"], Qt::Horizontal, tr("Full name"));
    queryModelWorker->setHeaderData(TA["phoneNumber"], Qt::Horizontal, tr("Phone number"));
    queryModelWorker->setHeaderData(TA["email"], Qt::Horizontal, tr("Email"));
    queryModelWorker->setHeaderData(TA["remarke"], Qt::Horizontal, tr("Remarke"));
    queryModelWorker->setHeaderData(TA["isDisponible"], Qt::Horizontal, tr("disponibility"));
    queryModelWorker->setHeaderData(TA["date_created"], Qt::Horizontal, tr("Date Created"));

    proxyModelWorker = new QSortFilterProxyModel(this);
    proxyModelWorker->setSourceModel(queryModelWorker);
    ui->tableViewWorkers->setModel(proxyModelWorker);

//    proxyModelWorker->setFilterRegExp(QRegExp("", Qt::CaseInsensitive,QRegExp::FixedString));
//    proxyModelWorker->setFilterKeyColumn(1);

    ui->tableViewWorkers->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableViewWorkers->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableViewWorkers->setSortingEnabled(true);
    ui->tableViewWorkers->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableViewWorkers->horizontalHeader()->stretchLastSection();
    ui->tableViewWorkers->verticalHeader()->stretchLastSection();
    ui->tableViewWorkers->resizeColumnsToContents();
    ui->tableViewWorkers->show();

    mapper->setSubmitPolicy(mapper->ManualSubmit);
    createMapper();

    ui->SB_idWorkerx->setVisible(false);

    ui->BT_cancelInfoW->setGraphicsEffect(Style::shadowbutton());
    ui->BT_saveWorker->setGraphicsEffect(Style::shadowbutton());
    ui->widgetSearch->setGraphicsEffect(Style::shadowbutton());
    ui->BT_updateWorker->setGraphicsEffect(Style::shadowbutton());
    ui->BT_delete->setGraphicsEffect(Style::shadowbutton());

    // Actions.
    connect(ui->BT_saveWorker,SIGNAL(clicked()),this,SLOT(saveDemande2DB()));
    connect(ui->BT_saveWorker,SIGNAL(clicked(bool)),ui->BT_saveWorker,SLOT(setDisabled(bool)));
    connect(ui->BT_cancelInfoW,SIGNAL(clicked()),this,SLOT(clearForm()));
    connect(ui->LE_fullnameFiltre,SIGNAL(textChanged(QString)),proxyModelWorker,SLOT(setFilterRegExp(QString)));
    connect(ui->tableViewWorkers->selectionModel(), SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),
            mapper, SLOT(setCurrentModelIndex(QModelIndex)));
    connect(ui->BT_delete,SIGNAL(clicked()),this,SLOT(deleteWorker()));

    connect(ui->BT_updateWorker,SIGNAL(clicked()),this,SLOT(updateWorker()));
    connect(ui->CB_disponibility,SIGNAL(currentIndexChanged(int)),
            this,SLOT(changeIconDisponibility()));

    connect(ui->SB_idWorkerx,SIGNAL(valueChanged(QString)),
            ui->LB_displayIdWx,SLOT(setText(QString)));
}

void TabWidgetWorkers::updateWorker()
{
    int idWorker = ui->SB_idWorkerx->value();
    if(idWorker != 0)
    {
        Worker* wrk = new Worker();
        wrk->setID(ui->SB_idWorkerx->value());
        wrk->setFullName(ui->LE_fullNamex->text());
        wrk->setPhoneNumber(ui->LE_phonex->text());
        wrk->setEmail(ui->LE_primeryEmlx->text());
        wrk->setRemarke(ui->TE_remarkex->toPlainText());
        wrk->setIsDisponible(ui->CB_disponibility->currentIndex() == 0);

        DB->updateWorker(wrk);

        queryModelWorker->setQuery("SELECT `id`,`fullName`,`phoneNumber`,"
                                   "`email`,`remarke`,`isDisponible`,"
                                   "`date_created` FROM `workers` ",DB->getCnx());
        clearFormUpdate();
        emit dataWorkersChanged();

    }else{
        Toast *mToast = new Toast(this);
        mToast->show(tr("Select Worker You Want To Update From The Table."),"");
    }

}

Worker *TabWidgetWorkers::getDatafromForm()
{
    Worker* data = new Worker();
    data->setFullName(ui->LE_fullNameWorker->text());
    data->setPhoneNumber(ui->LE_phoneWorker->text());
    data->setEmail(ui->LE_primeryEmailW->text());
    data->setRemarke(ui->TE_remarke->toPlainText());
    data->setDateCreated(QDateTime::currentDateTime().toString(Qt::ISODate));
    data->setDateModified(data->getDateCreated());
    return data;
}

void TabWidgetWorkers::changeIconDisponibility()
{
    if(ui->CB_disponibility->currentIndex() == 0)
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

bool TabWidgetWorkers::addWorker(Worker *data)
{
    int idWorker = DB->addWorker(data);

    queryModelWorker->setQuery("SELECT `id`,`fullName`,`phoneNumber`,"
                               "`email`,`remarke`,`isDisponible`,"
                               "`date_created` FROM `workers` ",DB->getCnx());
    emit dataWorkersChanged();

    if(idWorker > -1)
        return true;
    else
        return false;
}

bool TabWidgetWorkers::verificationSubmitedData(Worker *data)
{
    bool ver = true;
    if(data->getFullName().isEmpty()) ver = false;
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
    Worker* data = getDatafromForm();

    if(verificationSubmitedData(data))
    {
        Dialog *D = new Dialog(ui->T_addWorker);
        D->setMessage(tr("sure, save this worker"),"question");
        ui->GB_addWorker->setDisabled(true);
        if(D->exec()==1)
        {
        addWorker(data);
        clearForm();
        ui->BT_saveWorker->setEnabled(true);
        }
        ui->GB_addWorker->setEnabled(true);
    }
    else
    {
        Toast *mToast = new Toast(this);
        mToast->show(tr("atleast give the workerName"),"");
        ui->BT_saveWorker->setEnabled(true);
    }
}

void TabWidgetWorkers::createMapper(){

    mapper->setModel(proxyModelWorker);

    mapper->addMapping(ui->SB_idWorkerx,TA["id"]);
    mapper->addMapping(ui->LE_fullNamex, 1); // TA["fullName"]
    mapper->addMapping(ui->LE_phonex, TA["phoneNumber"]);
    mapper->addMapping(ui->LE_primeryEmlx, TA["email"]);
    mapper->addMapping(ui->TE_remarkex, TA["remarke"]);
    mapper->addMapping(ui->CB_disponibility, TA["isDisponible"]);
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
    if(ui->SB_idWorkerx->value() != 0)
    {
    Dialog *D = new Dialog(ui->T_updateWorker);
    D->setMessage(tr("sure, delete this worker"),"question");
    ui->GB_updateWorker->setDisabled(true);
    if(D->exec()==1)
    {
        DB->deleteWorkerbyID(ui->SB_idWorkerx->value());

        queryModelWorker->setQuery("SELECT `id`,`fullName`,`phoneNumber`,"
                                   "`email`,`remarke`,`isDisponible`,"
                                   "`date_created` FROM `workers` ",DB->getCnx());
        clearFormUpdate();
        emit dataWorkersChanged();
    }
    }else{
        Toast *mToast = new Toast(this);
        mToast->show(tr("select worker you want to delete from the table"),"");
    }
    ui->GB_updateWorker->setEnabled(true);
}
