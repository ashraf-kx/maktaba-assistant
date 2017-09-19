#include "tabwidgetclients.h"
#include "ui_tabwidgetclients.h"
#include <QDebug>
#include <QGraphicsDropShadowEffect>
#include <QPagedPaintDevice>
#include <QDir>
#include "dialog.h"
#include "toast.h"

Q_LOGGING_CATEGORY(CLN,"CLIENT")

TabWidgetClients::TabWidgetClients(QWidget *parent) :
    QTabWidget(parent),
    ui(new Ui::TabWidgetClients)
{
    ui->setupUi(this);

    DB               = new DBH("_clients_");

    mapper           = new QDataWidgetMapper();
    queryModelClient = new QSqlQueryModel();

    queryModelClient->setQuery("SELECT `id`,`fullname`,`phoneNumber`,"
                               "`firstEmail`,`isDeliveredByMail`,`remarke`,"
                               "`payement_state`,`price`,`pricePaid`,"
                               "`date_created`,`date_modified` FROM `clients` ",DB->getCnx());

    TA.clear();
    TA["id"]            = 0;
    TA["fullname"]      = 1;
    TA["phoneNumber"]   = 2;
    TA["firstEmail"]    = 3;
    TA["isDeliveredByMail"] = 4;
    TA["remarke"]       = 5;
    TA["payement_state"]= 6;
    TA["price"]         = 7;
    TA["pricePaid"]     = 8;
    TA["date_created"]  = 9;

    queryModelClient->setHeaderData(TA["id"], Qt::Horizontal, tr("N#"));
    queryModelClient->setHeaderData(TA["fullname"], Qt::Horizontal, tr("Client name"));
    queryModelClient->setHeaderData(TA["phoneNumber"], Qt::Horizontal, tr("Phone number"));
    queryModelClient->setHeaderData(TA["firstEmail"], Qt::Horizontal, tr("email"));
    queryModelClient->setHeaderData(TA["isDeliveredByMail"], Qt::Horizontal, tr("isDeliveredByMail"));
    queryModelClient->setHeaderData(TA["remarke"], Qt::Horizontal, tr("remarke"));
    queryModelClient->setHeaderData(TA["payement_state"], Qt::Horizontal, tr("payement state"));
    queryModelClient->setHeaderData(TA["price"], Qt::Horizontal, tr("price"));
    queryModelClient->setHeaderData(TA["pricePaid"], Qt::Horizontal, tr("price Paid"));
    queryModelClient->setHeaderData(TA["date_created"], Qt::Horizontal, tr("Date Created"));

    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);

    ui->tableView->setSortingEnabled(true);
    ui->tableView->setSelectionMode(QAbstractItemView::SingleSelection);

    proxyModelClient = new QSortFilterProxyModel(this);
    proxyModelClient->setSourceModel(queryModelClient);
    ui->tableView->setModel(proxyModelClient);

//    proxyModelClient->setFilterRegExp(QRegExp("", Qt::CaseInsensitive,QRegExp::FixedString));
//    proxyModelClient->setFilterKeyColumn(1);

    ui->tableView->horizontalHeader()->resizeSections(QHeaderView::ResizeToContents);
    ui->tableView->horizontalHeader()->stretchLastSection();
    ui->tableView->verticalHeader()->stretchLastSection();
    ui->tableView->resizeColumnsToContents();
    ui->tableView->resizeRowsToContents();
    ui->tableView->horizontalHeader()->setResizeContentsPrecision(0);
    ui->tableView->setWordWrap(true);
    ui->tableView->setVisible(true);

    ui->DE_deposite->setDate(QDate::currentDate());
    ui->DE_delivery->setDate(QDate::currentDate().addDays(1));

    mapper->setSubmitPolicy(mapper->ManualSubmit);
    createMapper();

    initListRowsPerPage();
    ui->Cb_rows->setCurrentIndex(0);

    ui->SB_idClientx->setVisible(false);

    ui->BT_save->setGraphicsEffect(Style::shadowbutton());
    ui->BT_cancel_2->setGraphicsEffect(Style::shadowbutton());
    ui->Bt_deleteClient->setGraphicsEffect(Style::shadowbutton());
    ui->BT_updateClient->setGraphicsEffect(Style::shadowbutton());
    ui->widgetSearch->setGraphicsEffect(Style::shadowbutton());
    ui->Bt_printTicket->setGraphicsEffect(Style::shadowbutton());

    initCalendar();

    // Actions.
    connect(ui->BT_save,SIGNAL(clicked()),this,SLOT(saveDemande2DB()));
    connect(ui->BT_save,SIGNAL(clicked(bool)),ui->BT_save,SLOT(setDisabled(bool)));
    connect(ui->BT_cancel_2,SIGNAL(clicked()),this,SLOT(clearForm()));
    connect(ui->Le_search,SIGNAL(textChanged(QString)),proxyModelClient,SLOT(setFilterRegExp(QString)));
    connect(ui->tableView->selectionModel(), SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),
            this, SLOT(setmModelIndex(QModelIndex,QModelIndex)));
    connect(ui->Bt_deleteClient,SIGNAL(clicked()),this,SLOT(deleteClient()));
    connect(ui->Bt_printTicket,SIGNAL(clicked()),this,SLOT(printTicket()));

    connect(ui->BT_updateClient,SIGNAL(clicked()),this,SLOT(updateClient()));

    connect(ui->DE_deposite,SIGNAL(dateChanged(const QDate &)),
            this,SLOT(updateMinDeliveryDay(const QDate &)));

    connect(ui->SB_priceAdv,SIGNAL(valueChanged(int)),this,SLOT(restPrice()));
    connect(ui->SB_price,SIGNAL(valueChanged(int)),this,SLOT(restPrice()));

    connect(ui->SB_priceAdvx,SIGNAL(valueChanged(int)),this,SLOT(restPricex()));
    connect(ui->SB_pricex,SIGNAL(valueChanged(int)),this,SLOT(restPricex()));

    connect(ui->SB_idClientx,SIGNAL(valueChanged(QString)),
            ui->LB_displayIdCx,SLOT(setText(QString)));

    connect(ui->Bt_edit,SIGNAL(pressed()),ui->widget,SLOT(hide()));
    connect(ui->Bt_edit,SIGNAL(released()),ui->widget,SLOT(show()));

    // Table View Data Contolled.
    connect(ui->Cb_rows,SIGNAL(currentTextChanged(QString)),this,SLOT(updateTableViewRows()));
    connect(ui->Cb_pages,SIGNAL(currentTextChanged(QString)),this,SLOT(showPageRows()));

    connect(ui->Le_search,SIGNAL(textChanged(QString)),
            proxyModelClient,SLOT(setFilterRegExp(QString)));

    connect(ui->Bt_previous,SIGNAL(clicked(bool)),this,SLOT(previousPage()));
    connect(ui->Bt_next,SIGNAL(clicked(bool)),this,SLOT(nextPage()));

    // connect(ui->Bt_editRow,SIGNAL(clicked(bool)),this,SLOT(visibility()));
}

void TabWidgetClients::updateClient()
{
    int idClient = ui->SB_idClientx->value();
    if(idClient != 0)
    {
        if(ui->SB_priceAdvx->value() <= ui->SB_pricex->value())
        {
            Client* data = new Client();

            data->setID(ui->SB_idClientx->value());
            data->setFullname(ui->LE_fullNamex->text());
            data->setPhoneNumber(ui->LE_phonex->text());
            data->setEmail(ui->LE_primeryEmlx->text());
            data->setPayement_state(ui->CB_paymentStatsx->currentText());
            data->setPrice(ui->SB_pricex->value());
            data->setPricePaid(ui->SB_priceAdvx->value());

            DB->updateClient(data);

            queryModelClient->setQuery("SELECT `id`,`fullname`,`phoneNumber`,"
                                       "`firstEmail`,`isDeliveredByMail`,`remarke`,"
                                       "`payement_state`,`price`,`pricePaid`,`date_created`,"
                                       "`date_modified` FROM `clients` ",DB->getCnx());

            clearFormUpdate();

            emit dataClientsChanged();
        }else{
            Toast *mToast = new Toast(this);
            mToast->show(tr("error value price paid more then total price "),"");
        }
    }else{
        Toast *mToast = new Toast(this);
        mToast->show(tr("select Client you want to update from the table"),"");
    }

}

void TabWidgetClients::restPrice()
{
    if(ui->SB_priceAdv->value() <= ui->SB_price->value())
    {
        ui->Lb_displayRestDZ->setText(tr("DZD")+"  "+
                                      QString::number(ui->SB_price->value()-ui->SB_priceAdv->value()));
        if(ui->SB_price->value()-ui->SB_priceAdv->value() == 0)
            ui->CB_payementState->setCurrentIndex(1);
        else
            ui->CB_payementState->setCurrentIndex(0);
    }
    else ui->Lb_displayRestDZ->setText(tr("DZD")+"  *  ");
}

void TabWidgetClients::restPricex()
{
    if(ui->SB_priceAdvx->value() <= ui->SB_pricex->value())
    {
        ui->Lb_priceRestDZx->setText(tr("DZD")+"  "+
                                      QString::number(ui->SB_pricex->value()-ui->SB_priceAdvx->value()));
        if(ui->SB_pricex->value()-ui->SB_priceAdvx->value() == 0)
            ui->CB_paymentStatsx->setCurrentIndex(1);
        else
            ui->CB_paymentStatsx->setCurrentIndex(0);
    }
    else ui->Lb_priceRestDZx->setText(tr("DZD")+"  *  ");
}

TabWidgetClients::~TabWidgetClients()
{
    delete ui;
}

Client* TabWidgetClients::getDatafromForm()
{
    Client *data = new Client();
    data->setFullname(ui->LE_fullnameClient->text());
    data->setRemarke(ui->TE_remarke->toPlainText());
    data->setPayement_state(ui->CB_payementState->currentText());
    data->setPhoneNumber(ui->LE_phoneNumber->text());
    data->setEmail(ui->LE_primaryEmail->text());

    Document *doc = new Document();
    doc->setTitleDoc(ui->LE_titleDoc->text());
    doc->setTotalPages(ui->SB_nbrPagesDoc->value());
    doc->setDepositeDay(ui->DE_deposite->date().toString("yyyy-MM-dd"));
    doc->setDeliveryDay(ui->DE_delivery->date().toString("yyyy-MM-dd"));
    doc->setIsDeliveredByMail(false);
    doc->setFontFamily(ui->CB_fontFamily->currentText());
    doc->setFontSize(ui->CB_fontSize->currentText().toInt());
    doc->setTypeDoc(ui->CB_catDoc->currentText());
    doc->setPrintingColor(tr("BW"));
    doc->setLanguageDoc(ui->CB_langDoc->currentText());
    doc->setDateCreated(QDateTime::currentDateTime().toString(Qt::ISODate));
    doc->setDateModified(doc->getDateCreated());

    data->setPrice(ui->SB_price->value());
    data->setPricePaid(ui->SB_priceAdv->value());

    data->setDateCreated(QDateTime::currentDateTime().toString(Qt::ISODate));
    data->setDateModified(data->getDateCreated());

    data->setDocument(doc);
    return data;
}

void TabWidgetClients::initCalendar()
{
    ui->DE_deposite->setCalendarPopup(true);
    ui->DE_deposite->setMinimumDate(QDate::currentDate());
    ui->DE_deposite->setCalendarWidget(ui->datePicker1);

    ui->DE_delivery->setCalendarPopup(true);
    ui->DE_delivery->setMinimumDate(QDate::currentDate().addDays(1));
    ui->DE_delivery->setCalendarWidget(ui->datePicker2);

    QGraphicsDropShadowEffect *sh = new QGraphicsDropShadowEffect();
    sh->setBlurRadius(8);
    sh->setOffset(2);
    sh->setColor(QColor(63, 63, 63, 180));

    ui->datePicker1->setGraphicsEffect(sh);

    QGraphicsDropShadowEffect *sh1 = new QGraphicsDropShadowEffect();
    sh1->setBlurRadius(8);
    sh1->setOffset(2);
    sh1->setColor(QColor(63, 63, 63, 180));

    ui->datePicker2->setGraphicsEffect(sh1);
}

void TabWidgetClients::updateMinDeliveryDay(const QDate &date)
{
    ui->DE_delivery->setMinimumDate(ui->DE_deposite->date().addDays(1));
}

bool TabWidgetClients::addClient(Client* data)
{
    int idClient = DB->addClient(data);
    DB->addDocument(idClient,data->getDocument());

    queryModelClient->setQuery("SELECT `id`,`fullname`,`phoneNumber`,`firstEmail`,`isDeliveredByMail`,"
                               " `remarke`,`payement_state`,`price`,`pricePaid`,`date_created`,`date_modified` FROM `clients` ",DB->getCnx());
    emit dataClientsChanged();
    return true;
}

bool TabWidgetClients::verificationSubmitedData(Client* mClientData)
{
    bool ver = true;
    if(mClientData->getFullname().isEmpty()) ver = false;
    if(mClientData->getDocument()->getTitleDoc().isEmpty()) ver = false;
    return ver;
}

void TabWidgetClients::clearForm()
{
    ui->LE_fullnameClient->clear();
    ui->LE_titleDoc->clear();
    ui->SB_nbrPagesDoc->setValue(1);
    ui->DE_deposite->setDate(QDate::currentDate());
    ui->DE_delivery->setDate(QDate::currentDate().addDays(1));
    ui->TE_remarke->clear();
    ui->CB_payementState->setCurrentIndex(0);
    ui->LE_phoneNumber->clear();
    ui->LE_primaryEmail->clear();
    ui->SB_price->setValue(0);
    ui->CB_langDoc->setCurrentIndex(0);
}

void TabWidgetClients::clearFormUpdate()
{
    ui->LB_displayIdCx->setText("*");
    ui->SB_idClientx->setValue(0);
    ui->LE_fullNamex->clear();
    ui->LE_phonex->clear();
    ui->LE_primeryEmlx->clear();
    ui->SB_pricex->setValue(0);
    ui->SB_priceAdvx->setValue(0);
    ui->CB_paymentStatsx->setCurrentIndex(2);
}

void TabWidgetClients::saveDemande2DB()
{
    Client *data = getDatafromForm();

    if(verificationSubmitedData(data))
    {
        MDialog *mDialog = new MDialog(this);
        mDialog->setMessage(tr("Do You Really Want To Add This Client ?"));
        if(mDialog->exec() == 1 )
        {
            addClient(data);
            clearForm();
            ui->BT_save->setEnabled(true);
        }
        mDialog->close();
        mDialog->deleteLater();
        ui->GB_addClient->setEnabled(true);
    }
    else
    {
        Toast *mToast = new Toast(this);
        mToast->show(tr("Atleast Give The Client A Name And TitleDoc"),"");
        ui->BT_save->setEnabled(true);
    }
}

void TabWidgetClients::createMapper()
{
    mapper->setModel(proxyModelClient);

    mapper->addMapping(ui->SB_idClientx,TA["id"]);
    mapper->addMapping(ui->LE_fullNamex,TA["fullname"]);
    mapper->addMapping(ui->LE_phonex, TA["phoneNumber"]);
    mapper->addMapping(ui->LE_primeryEmlx, TA["firstEmail"]);
    mapper->addMapping(ui->CB_paymentStatsx, TA["payement_state"]);
    mapper->addMapping(ui->SB_pricex, TA["price"]);
    mapper->addMapping(ui->SB_priceAdvx, TA["pricePaid"]);
}

void TabWidgetClients::deleteClient()
{
    int idClient = ui->SB_idClientx->value();
    if(idClient != 0)
    {
        Dialog *D = new Dialog(ui->T_updateClient);
        D->setMessage(tr("sure, delete this client"),"question");
        ui->GB_updateClient->setDisabled(true);
        if(D->exec()==1)
        {
            DB->deleteClientByID(idClient);

            queryModelClient->setQuery("SELECT `id`, `fullname`, `phoneNumber`, `firstEmail`, `isDeliveredByMail`,"
                                       " `remarke`, `payement_state`, `price`, `pricePaid`, `date_created`, `date_modified` FROM `clients` ",DB->getCnx());

            clearFormUpdate();
            emit dataClientsChanged();
        }
        }else{
            Toast *mToast = new Toast(this);
            mToast->show(tr("select Client you want to delete from the table"),"");
        }
    ui->GB_updateClient->setDisabled(false);
}

void TabWidgetClients::printTicket()
{
//    int idClient = ui->SB_idClientx->value();
//    if(idClient != 0){

//    QString CName,CDocN,DateDP,DateDV,pages,lang,phoneN,AdressLocal;

//    this->DBH.open();
//    this->DBH.transaction();
//    QSqlQuery *query = new QSqlQuery(this->DBH);

//    query->exec("SELECT fullname FROM clients WHERE id="+QString::number(idClient));
//    if(query->next())
//        CName = query->value(0).toString();

//    query->exec("SELECT titleDoc,languageDoc,totalPages"
//                ",depositeDay,deliveryDay FROM documents "
//                "WHERE idClient="+QString::number(idClient));
//    if(query->next()){
//        CDocN  = query->value(0).toString();
//        lang   = query->value(1).toString();
//        pages  = query->value(2).toString();
//        DateDP = query->value(3).toString();
//        DateDV = query->value(4).toString();
//    }

//    query->exec("SELECT phoneNumber,localAdress FROM admin WHERE app_name='yassin' ");
//    if(query->next()){
//        phoneN = query->value(0).toString();
//        AdressLocal = query->value(1).toString();
//    }

//    this->DBH.commit();
//    this->DBH.close();

//    QString Ticket = tr("N : ")+QString::number(idClient)+"\n"+
//            tr("Full Name : ")+CName+"\n"+
//            tr("Title : ")+CDocN+"\n"+
//            "\n"+
//            tr("Deposite day : ")+DateDP+"\n"+
//            tr("Delivery day : ")+DateDV+"\n"+
//            "\n\n";

//    QPrinter printer;
//    printer.setFullPage(true);
//    printer.setPaperSize(QPrinter::A7);
//    printer.setPageSize(QPrinter::A7);
//    printer.setOrientation(QPrinter::Landscape);
//    printer.setOutputFileName(QString::number(QDateTime::currentDateTime().toMSecsSinceEpoch())+"Ticket_"+QString::number(idClient)+".oxps");


//    QPagedPaintDevice::Margins margs;
//    margs.top = 1.0;
//    margs.left = 2.0;
//    margs.right = 2.0;
//    margs.bottom=1.0;
//    printer.setMargins(margs);

//        QPrintDialog *dialog = new QPrintDialog(&printer);

//        dialog->setWindowTitle(tr("Print Your Ticket"));

//        if (dialog->exec() == QDialog::Accepted)
//        {
//            QPainter *painter = new QPainter();
//            if(painter->begin(&printer))
//            {
//            painter->drawPixmap(12,12,60,60,QPixmap(":Icons/ic_local_library_black_48dp.png"));
//            painter->setPen(Qt::black);
//            painter->setFont(QFont("Arial", 12));
//            painter->drawPixmap(345,210,22,22,QPixmap(":Icons/ic_domain_2x.png"));
//            painter->drawText(-63, 210, 385, 35,  Qt::AlignRight , AdressLocal);
//            painter->drawPixmap(345,235,22,22,QPixmap(":Icons/ic_call_2x.png"));
//            painter->drawText(-63, 235, 385, 35,  Qt::AlignRight , phoneN);

//            painter->setPen(Qt::black);
//            painter->setFont(QFont("Arial", 17));
//            painter->drawText(150, 9, 385, 270,  Qt::AlignTop , tr("library yassine"));

//            painter->drawLine(80,45,380,45);
//            painter->drawLine(80,46,380,46);

//            painter->drawLine(10,200,380,200);
//            painter->drawLine(10,201,380,201);


//            painter->setPen(Qt::black);
//            painter->setFont(QFont("Arial", 14));

//            painter->drawRoundRect(0,0,385,265);
//            painter->drawRoundRect(1,1,384,264);

//            painter->drawText(-60, 48, 385, 265, Qt::AlignRight|Qt::AlignTop , Ticket);

//            painter->end();
//            }else{
//                Toast *mToast = new Toast(this);
//                mToast->setMessage(tr("Error repeat printing ticket"));
//            }

//        }
//    }else{
//        Toast *mToast = new Toast(this);
//        mToast->setMessage(tr("select Client you want to print his ticket"));
//    }

}

// Controle the table View Data, show only 15 line and so on .
void TabWidgetClients::setmModelIndex(QModelIndex idx,QModelIndex idx2)
{
    Q_UNUSED(idx2);
    mapper->setCurrentModelIndex(idx);
}

void TabWidgetClients::updateMessageInfo()
{
    int startIdx =0;
    int numRowsVisible =0;
    int modelRows = queryModelClient->rowCount();
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

void TabWidgetClients::nextPage()
{
    int idxCurrent = ui->Cb_pages->currentIndex();
    if(idxCurrent < ui->Cb_pages->count()-1 )
    {
        ui->Cb_pages->setCurrentIndex(idxCurrent+1);
        updateMessageInfo();
    }
}

void TabWidgetClients::previousPage()
{
    int idxCurrent = ui->Cb_pages->currentIndex();
    if(idxCurrent > 0 )
    {
        ui->Cb_pages->setCurrentIndex(idxCurrent-1);
        updateMessageInfo();
    }
}

void TabWidgetClients::selectedColumn(QModelIndex idx,QModelIndex idx2)
{
    Q_UNUSED(idx2)
    qDebug()<<"row : "<<idx.row()<<"Collumn : "<<idx.column();

   idxColSelected = idx.column();//ui->tableView->currentIndex().column();
   if(idxColSelected >= 0 )
   {
       proxyModelClient->setFilterRegExp(QRegExp("", Qt::CaseInsensitive));
       proxyModelClient->setFilterKeyColumn(idxColSelected);
       ui->Le_search->setPlaceholderText(tr("Search By ")+queryModelClient->headerData(idxColSelected,Qt::Horizontal,Qt::DisplayRole).toString());
   }
}

void TabWidgetClients::hideAllRows()
{
    for(int i=0; i<queryModelClient->rowCount(); i++)
    {
        ui->tableView->hideRow(i);
    }
}

void TabWidgetClients::showAllRows()
{
    for(int i=0; queryModelClient->rowCount();i++)
    {
        ui->tableView->showRow(i);
    }
}

void TabWidgetClients::initListRowsPerPage()
{
    ui->Cb_rows->clear();
    int maxRows  = queryModelClient->rowCount();
    ui->Cb_rows->addItem(QString::number(maxRows));

    if(maxRows >= 10 ) ui->Cb_rows->addItem("10");
    if(maxRows >= 15 ) ui->Cb_rows->addItem("15");
    if(maxRows >= 20 ) ui->Cb_rows->addItem("20");
    if(maxRows >= 30 ) ui->Cb_rows->addItem("30");
    if(maxRows >= 50 ) ui->Cb_rows->addItem("50");
    if(maxRows >= 100 ) ui->Cb_rows->addItem("100");
}

void TabWidgetClients::initListNumberPages()
{
    ui->Cb_pages->clear();
    int maxRows   = queryModelClient->rowCount();
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

void TabWidgetClients::showPageRows()
{
    hideAllRows();
    int startIdx =0;
    int numRowsVisible =0;
    if(ui->Cb_pages->count()>0 && ui->Cb_rows->count()>0)
    {
        startIdx = (ui->Cb_pages->currentText().toInt()-1)*ui->Cb_rows->currentText().toInt();
        numRowsVisible = ui->Cb_rows->currentText().toInt();

        int endIdx = startIdx+numRowsVisible;
        if(endIdx>queryModelClient->rowCount()) endIdx = queryModelClient->rowCount();

        for(int i = startIdx; i< endIdx; i++)
        {
            ui->tableView->showRow(i);
        }
        updateMessageInfo();
    }
}

void TabWidgetClients::updateTableViewRows()
{
    initListNumberPages();
    showPageRows();
}
