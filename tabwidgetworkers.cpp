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

    DB = new DBH();

    queryModelWorker = new QSqlQueryModel();
    queryModelWorker->setQuery("SELECT id,fullName,phoneNumber,"
                               "email,remarke,isDisponible,"
                               "date_created FROM workers ",DB->getConnection());

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
    ui->tableView->setModel(proxyModelWorker);

//    proxyModelWorker->setFilterRegExp(QRegExp("", Qt::CaseInsensitive,QRegExp::FixedString));
//    proxyModelWorker->setFilterKeyColumn(1);

    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableView->setSortingEnabled(true);
    ui->tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableView->horizontalHeader()->stretchLastSection();
    ui->tableView->verticalHeader()->stretchLastSection();
    ui->tableView->resizeColumnsToContents();
    ui->tableView->show();

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
    connect(ui->Le_search,SIGNAL(textChanged(QString)),proxyModelWorker,SLOT(setFilterRegExp(QString)));
    connect(ui->tableView->selectionModel(), SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),
            mapper, SLOT(setCurrentModelIndex(QModelIndex)));
    connect(ui->BT_delete,SIGNAL(clicked()),this,SLOT(deleteWorker()));

    connect(ui->BT_updateWorker,SIGNAL(clicked()),this,SLOT(updateWorker()));
    connect(ui->CB_disponibility,SIGNAL(currentIndexChanged(int)),
            this,SLOT(changeIconDisponibility()));

    connect(ui->SB_idWorkerx,SIGNAL(valueChanged(QString)),
            ui->LB_displayIdWx,SLOT(setText(QString)));


    // Table View Data Contolled.
    connect(ui->Bt_edit,SIGNAL(pressed()),ui->widget,SLOT(hide()));
    connect(ui->Bt_edit,SIGNAL(released()),ui->widget,SLOT(show()));

    initListRowsPerPage();
    ui->Cb_rows->setCurrentIndex(0);

    connect(ui->Cb_rows,SIGNAL(currentTextChanged(QString)),this,SLOT(updateTableViewRows()));
    connect(ui->Cb_pages,SIGNAL(currentTextChanged(QString)),this,SLOT(showPageRows()));

    connect(ui->Le_search,SIGNAL(textChanged(QString)),
            proxyModelWorker,SLOT(setFilterRegExp(QString)));

    connect(ui->Bt_previous,SIGNAL(clicked(bool)),this,SLOT(previousPage()));
    connect(ui->Bt_next,SIGNAL(clicked(bool)),this,SLOT(nextPage()));
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

        queryModelWorker->setQuery("SELECT id,fullName,phoneNumber,"
                                   "email,remarke,isDisponible,"
                                   "date_created FROM workers ",DB->getConnection());
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
    data->setDateModified(QDateTime::currentDateTime().toString(Qt::ISODate));
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

    queryModelWorker->setQuery("SELECT id,fullName,phoneNumber,"
                               "email,remarke,isDisponible,"
                               "date_created FROM workers ",DB->getConnection());
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

        queryModelWorker->setQuery("SELECT id,fullName,phoneNumber,"
                                   "email,remarke,isDisponible,"
                                   "date_created FROM workers ",DB->getConnection());
        clearFormUpdate();
        emit dataWorkersChanged();
    }
    }else{
        Toast *mToast = new Toast(this);
        mToast->show(tr("select worker you want to delete from the table"),"");
    }
    ui->GB_updateWorker->setEnabled(true);
}

// Controle the table View Data, show only 15 line and so on .
void TabWidgetWorkers::setmModelIndex(QModelIndex idx,QModelIndex idx2)
{
    Q_UNUSED(idx2);
    mapper->setCurrentModelIndex(idx);
}

void TabWidgetWorkers::updateMessageInfo()
{
    int startIdx =0;
    int numRowsVisible =0;
    int modelRows = queryModelWorker->rowCount();
    if(ui->Cb_pages->count()>0 && ui->Cb_rows->count()>0)
    {
        startIdx = (ui->Cb_pages->currentText().toInt()-1)*ui->Cb_rows->currentText().toInt();
        numRowsVisible = ui->Cb_rows->currentText().toInt();

        int endIdx = startIdx+numRowsVisible;
        if(endIdx>modelRows) endIdx = modelRows;
        ui->L_info->setText(tr("Showing ")+QString::number(startIdx+1)+
                            tr(" to ")+QString::number(endIdx)+
                            tr(" From ")+QString::number(modelRows)+
                            tr(" Entries."));
    }
}

void TabWidgetWorkers::nextPage()
{
    int idxCurrent = ui->Cb_pages->currentIndex();
    if(idxCurrent < ui->Cb_pages->count()-1 )
    {
        ui->Cb_pages->setCurrentIndex(idxCurrent+1);
        updateMessageInfo();
    }
}

void TabWidgetWorkers::previousPage()
{
    int idxCurrent = ui->Cb_pages->currentIndex();
    if(idxCurrent > 0 )
    {
        ui->Cb_pages->setCurrentIndex(idxCurrent-1);
        updateMessageInfo();
    }
}

void TabWidgetWorkers::selectedColumn(QModelIndex idx,QModelIndex idx2)
{
    Q_UNUSED(idx2)
    qDebug()<<"row : "<<idx.row()<<"Collumn : "<<idx.column();

   idxColSelected = idx.column();//ui->tableView->currentIndex().column();
   if(idxColSelected >= 0 )
   {
       proxyModelWorker->setFilterRegExp(QRegExp("", Qt::CaseInsensitive));
       proxyModelWorker->setFilterKeyColumn(idxColSelected);
       ui->Le_search->setPlaceholderText(tr("Search By ")+queryModelWorker->headerData(idxColSelected,Qt::Horizontal,Qt::DisplayRole).toString());
   }
}

void TabWidgetWorkers::hideAllRows()
{
    for(int i=0; i<queryModelWorker->rowCount(); i++)
    {
        ui->tableView->hideRow(i);
    }
}

void TabWidgetWorkers::showAllRows()
{
    for(int i=0; queryModelWorker->rowCount();i++)
    {
        ui->tableView->showRow(i);
    }
}

void TabWidgetWorkers::initListRowsPerPage()
{
    ui->Cb_rows->clear();
    int maxRows  = queryModelWorker->rowCount();
    ui->Cb_rows->addItem(QString::number(maxRows));

    if(maxRows >= 10 ) ui->Cb_rows->addItem("10");
    if(maxRows >= 15 ) ui->Cb_rows->addItem("15");
    if(maxRows >= 20 ) ui->Cb_rows->addItem("20");
    if(maxRows >= 30 ) ui->Cb_rows->addItem("30");
    if(maxRows >= 50 ) ui->Cb_rows->addItem("50");
    if(maxRows >= 100 ) ui->Cb_rows->addItem("100");
}

void TabWidgetWorkers::initListNumberPages()
{
    ui->Cb_pages->clear();
    int maxRows   = queryModelWorker->rowCount();
    int RowsCount = ui->Cb_rows->currentText().toInt();

    int NumPages  = maxRows/RowsCount;
    int restPages = maxRows%RowsCount;

    int save_i = 0;
    for(int i=1; i<= NumPages;i++)
    {
        ui->Cb_pages->addItem(QString::number(i));
        save_i = i;
    }

    if(restPages > 0) ui->Cb_pages->addItem(QString::number(save_i+1));
    updateMessageInfo();
}

void TabWidgetWorkers::showPageRows()
{
    hideAllRows();
    int startIdx =0;
    int numRowsVisible =0;
    if(ui->Cb_pages->count()>0 && ui->Cb_rows->count()>0)
    {
        startIdx = (ui->Cb_pages->currentText().toInt()-1)*ui->Cb_rows->currentText().toInt();
        numRowsVisible = ui->Cb_rows->currentText().toInt();

        int endIdx = startIdx+numRowsVisible;
        if(endIdx>queryModelWorker->rowCount()) endIdx = queryModelWorker->rowCount();

        for(int i = startIdx; i< endIdx; i++)
        {
            ui->tableView->showRow(i);
        }
        updateMessageInfo();
    }
}

void TabWidgetWorkers::updateTableViewRows()
{
    initListNumberPages();
    showPageRows();
}
