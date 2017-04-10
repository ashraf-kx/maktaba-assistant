#include "tabwidgetclients.h"
#include "ui_tabwidgetclients.h"
#include <QDebug>
#include <QGraphicsDropShadowEffect>
#include <QPagedPaintDevice>
#include <QDir>
#include "dialog.h"
#include "toast.h"


TabWidgetClients::TabWidgetClients(QWidget *parent) :
    QTabWidget(parent),
    ui(new Ui::TabWidgetClients)
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
    sh1->setOffset(2);
    sh1->setColor(QColor(63, 63, 63, 180));

    QGraphicsDropShadowEffect *sh3 = new QGraphicsDropShadowEffect();
    sh3->setBlurRadius(8);
    sh3->setOffset(2);
    sh3->setColor(QColor(63, 63, 63, 180));

    QGraphicsDropShadowEffect *sh4 = new QGraphicsDropShadowEffect();
    sh4->setBlurRadius(8);
    sh4->setOffset(2);
    sh4->setColor(QColor(63, 63, 63, 180));

    QGraphicsDropShadowEffect *sh5 = new QGraphicsDropShadowEffect();
    sh5->setBlurRadius(8);
    sh5->setOffset(2);
    sh5->setColor(QColor(63, 63, 63, 180));

    this->DBH = QSqlDatabase::addDatabase("QSQLITE","cnxnClient");
    this->DBH.setDatabaseName(QDir::homePath()+"/AppData/Roaming/bits/"+"BYASS.db");
    this->DBH.setPassword("bitProjects");
    this->DBH.setUserName("neverAsk@4Pass");
    this->DBH.open();

    mapper = new QDataWidgetMapper();
    modelClient = new QSqlTableModel(this,this->DBH);
    modelClient->setTable("clients");
    modelClient->select();


    QStringList listHeader;
    listHeader<<tr("Client name");
    listHeader<<tr("Phone number");
    listHeader<<tr("email");
    listHeader<<tr("isDeliveredByMail");
    listHeader<<tr("remarke");
    listHeader<<tr("payement state");
    listHeader<<tr("price");
    listHeader<<tr("price Paid");

    modelClient->setHeaderData(0, Qt::Horizontal, tr("N#"));
    modelClient->setHeaderData(1, Qt::Horizontal, listHeader.at(0));
    modelClient->setHeaderData(2, Qt::Horizontal, listHeader.at(1));
    modelClient->setHeaderData(3, Qt::Horizontal, listHeader.at(2));
    modelClient->setHeaderData(4, Qt::Horizontal, listHeader.at(3));
    modelClient->setHeaderData(5, Qt::Horizontal, listHeader.at(4));
    modelClient->setHeaderData(6, Qt::Horizontal, listHeader.at(5));
    modelClient->setHeaderData(7, Qt::Horizontal, listHeader.at(6));
    modelClient->setHeaderData(8, Qt::Horizontal, listHeader.at(7));

    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);

    ui->tableView->setSortingEnabled(true);
    ui->tableView->setSelectionMode(QAbstractItemView::SingleSelection);

    proxyModelClient = new QSortFilterProxyModel(this);

    proxyModelClient->setSourceModel(modelClient);

    proxyModelClient->setFilterRegExp(QRegExp("", Qt::CaseInsensitive,QRegExp::FixedString));
    proxyModelClient->setFilterKeyColumn(1);


    ui->tableView->setModel(proxyModelClient);

    ui->tableView->horizontalHeader()->stretchLastSection();
    ui->tableView->verticalHeader()->stretchLastSection();
    ui->tableView->resizeColumnsToContents();

    ui->tableView->show();

    ui->DE_deposite->setDate(QDate::currentDate());
    ui->DE_delivery->setDate(QDate::currentDate().addDays(1));

    ui->tableView->setColumnHidden(4,true);
    ui->tableView->setColumnHidden(9,true);
    ui->tableView->setColumnHidden(10,true);

    startMapper();
    ui->SB_idClientx->setVisible(false);

    ui->BT_save->setGraphicsEffect(sh);
    ui->BT_cancel_2->setGraphicsEffect(sh2);
    ui->Bt_deleteClient->setGraphicsEffect(sh1);
    ui->BT_updateClient->setGraphicsEffect(sh3);
    ui->widgetSearch->setGraphicsEffect(sh4);
    ui->Bt_printTicket->setGraphicsEffect(sh5);

    initCalendar();

    // Actions.
    connect(ui->BT_save,SIGNAL(clicked()),this,SLOT(saveDemande2DB()));
    connect(ui->BT_save,SIGNAL(clicked(bool)),ui->BT_save,SLOT(setDisabled(bool)));
    connect(ui->BT_cancel_2,SIGNAL(clicked()),this,SLOT(clearForm()));
    connect(ui->LE_fullnameFiltre,SIGNAL(textChanged(QString)),proxyModelClient,SLOT(setFilterRegExp(QString)));
    connect(ui->tableView->selectionModel(), SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),
            mapper, SLOT(setCurrentModelIndex(QModelIndex)));
    connect(ui->Bt_deleteClient,SIGNAL(clicked()),this,SLOT(deleteClient()));
    connect(ui->Bt_printTicket,SIGNAL(clicked()),this,SLOT(printTicket()));

    connect(ui->BT_updateClient,SIGNAL(clicked()),this,SLOT(updateClient()));

    connect(ui->DE_deposite,SIGNAL(dateChanged(const QDate &)),
            this,SLOT(updateMinDeliveryDay(const QDate &)));

    connect(ui->SB_priceAdv,SIGNAL(valueChanged(int)),this,SLOT(restPrice()));
    connect(ui->SB_price,SIGNAL(valueChanged(int)),this,SLOT(restPrice()));

    connect(ui->SB_priceAdvx,SIGNAL(valueChanged(int)),this,SLOT(restPricex()));
    connect(ui->SB_pricex,SIGNAL(valueChanged(int)),this,SLOT(restPricex()));
}

void TabWidgetClients::updateClient()
{
    proxyModelClient->submit();
    clearFormUpdate();
    emit dataClientsChanged();
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

TabWidgetClients::clientData TabWidgetClients::getDatafromForm()
{
    clientData mClientData;
    mClientData.fullname    = ui->LE_fullnameClient->text();
    mClientData.titleDoc    = ui->LE_titleDoc->text();
    mClientData.totalPages  = QString::number(ui->SB_nbrPagesDoc->value());
    mClientData.depositeDay = ui->DE_deposite->date().toString("yyyy-MM-dd");
    mClientData.deliveryDay = ui->DE_delivery->date().toString("yyyy-MM-dd");
    mClientData.remarke    = ui->TE_remarke->toPlainText();
    mClientData.payement_state = ui->CB_payementState->currentText();
    mClientData.phoneNumber = ui->LE_phoneNumber->text();
    mClientData.FirstEmail  = ui->LE_primaryEmail->text();
    mClientData.isDeliveredByMail = "yes" ;

    mClientData.fontFamily    = ui->CB_fontFamily->currentText();
    mClientData.fontSize      = ui->CB_fontSize->currentText();
    mClientData.typeDoc       = ui->CB_catDoc->currentText();
    mClientData.printingColor = "BW";

    mClientData.price         = QString::number(ui->SB_price->value());
    mClientData.pricePaid     = QString::number(ui->SB_priceAdv->value());
    mClientData.languageDoc   = ui->CB_langDoc->currentText();
    mClientData.date_created  = QDateTime::currentDateTime().toTime_t();
    mClientData.date_modified = mClientData.date_created;
    return mClientData;
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

bool TabWidgetClients::addClient(TabWidgetClients::clientData mClientData)
{
    this->DBH.open();
    this->DBH.transaction();

    //! [1] Save data into clients table.
    QSqlQuery *query = new QSqlQuery(this->DBH);
    query->prepare("INSERT INTO clients (id,fullname,phoneNumber,firstEmail,"
                   "remarke,payement_state,price,pricePaid,date_created,date_modified)"
                   " VALUES (NULL,:fullname,:phoneNumber,:firstEmail,"
                   ":remarke,:payement_state,:price,:pricePaid,:date_created,:date_modified)");

    query->bindValue(":fullname",mClientData.fullname);
    query->bindValue(":phoneNumber",mClientData.phoneNumber);
    query->bindValue(":firstEmail",mClientData.FirstEmail);

    query->bindValue(":remarke", mClientData.remarke);
    query->bindValue(":payement_state",mClientData.payement_state);
    query->bindValue(":price",mClientData.price);
    query->bindValue(":pricePaid",mClientData.pricePaid);
    query->bindValue(":date_created",mClientData.date_created);
    query->bindValue(":date_modified",mClientData.date_modified);

    query->exec();

    //! [2] Save data into *** documents table.
    QVariant lastIDVar = query->lastInsertId();
    int lastID = lastIDVar.toInt();
    query->prepare("INSERT INTO documents (id,idClient,titleDoc,languageDoc,"
                  "totalPages,fontFamily,fontSize,typeDoc,printingColor,isPrinted,depositeDay,deliveryDay,dateStarted,dateFinished,"
                  "date_created,date_modified) VALUES(NULL,:idClient,:titleDoc,:languageDoc,"
                  ":totalPages,:fontFamily,:fontSize,:typeDoc,:printingColor,:isPrinted,:depositeDay,:deliveryDay,:dateStarted,:dateFinished,"
                  ":date_created,:date_modified) ");

    query->bindValue(":idClient", QString::number(lastID));
    query->bindValue(":titleDoc", mClientData.titleDoc);
    query->bindValue(":fontFamily", mClientData.fontFamily);
    query->bindValue(":fontSize", mClientData.fontSize);
    query->bindValue(":typeDoc", mClientData.typeDoc);
    query->bindValue(":printingColor", mClientData.printingColor);
    query->bindValue(":isPrinted", tr("unprinted"));
    query->bindValue(":languageDoc", mClientData.languageDoc);
    query->bindValue(":totalPages", mClientData.totalPages);
    query->bindValue(":depositeDay", mClientData.depositeDay);
    query->bindValue(":deliveryDay", mClientData.deliveryDay);
    query->bindValue(":dateStarted", "-1");
    query->bindValue(":dateFinished", "-1");
    query->bindValue(":date_created", mClientData.date_created);
    query->bindValue(":date_modified", mClientData.date_modified);
    query->exec();

    this->DBH.commit();
    modelClient->select();
    emit dataClientsChanged();
    return true;

}

bool TabWidgetClients::verificationSubmitedData(TabWidgetClients::clientData mClientData)
{
    bool ver = true;
    if(mClientData.fullname.isEmpty()) ver = false;
    if(mClientData.titleDoc.isEmpty()) ver = false;
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
    ui->SB_idClientx->setValue(0);
    ui->LE_fullNamex->clear();
    ui->LE_phonex->clear();
    ui->LE_primeryEmlx->clear();
    ui->SB_pricex->setValue(0);
    ui->CB_paymentStatsx->setCurrentIndex(0);
}

void TabWidgetClients::saveDemande2DB()
{
    clientData mClientData = getDatafromForm();

    if(verificationSubmitedData(mClientData))
    {
        Dialog *D = new Dialog(ui->T_addClient);
        D->setMessage(tr("sure, save this informations ?"),"question");
        ui->GB_addClient->setDisabled(true);
        if(D->exec()==1)
        {
            addClient(mClientData);
            clearForm();
            ui->BT_save->setEnabled(true);

        }
        ui->GB_addClient->setEnabled(true);
    }
    else
    {
        Toast *mToast = new Toast(this);
        mToast->setMessage(tr("atleast give the clientname and titleDoc"));
        ui->BT_save->setEnabled(true);
    }
}

void TabWidgetClients::startMapper(){

    mapper->setModel(proxyModelClient);

    mapper->addMapping(ui->SB_idClientx,modelClient->fieldIndex("id"));
    mapper->addMapping(ui->LE_fullNamex, modelClient->fieldIndex("fullName"));
    mapper->addMapping(ui->LE_phonex, modelClient->fieldIndex("phoneNumber"));
    mapper->addMapping(ui->LE_primeryEmlx, modelClient->fieldIndex("firstEmail"));
    mapper->addMapping(ui->CB_paymentStatsx, modelClient->fieldIndex("payement_state"));
    mapper->addMapping(ui->SB_pricex, modelClient->fieldIndex("price"));
    mapper->addMapping(ui->SB_priceAdvx, modelClient->fieldIndex("pricePaid"));
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

        this->DBH.open();
        this->DBH.transaction();

        int idDoc = 0;
        int idWrkr= 0;

        //! [1] Save data into workers table.
        QSqlQuery *query = new QSqlQuery(this->DBH);
        query->prepare("SELECT id,idWorker FROM documents WHERE idClient='"+QString::number(idClient)+"' ");
        query->exec();
        while (query->next()) {
                idDoc  =  query->value(0).toInt();
                idWrkr =  query->value(1).toInt();
            }
        query->exec("UPDATE workers SET currentDocID=0 WHERE id='"+QString::number(idWrkr)+"'");
        query->exec("DELETE FROM documents WHERE id='"+QString::number(idDoc)+"'");
        query->exec("DELETE FROM clients WHERE id='"+QString::number(idClient)+"'");
        this->DBH.commit();

        modelClient->select();
        clearFormUpdate();
        emit dataClientsChanged();
    }
    }else{
        Toast *mToast = new Toast(this);
        mToast->setMessage(tr("select Client you want to delete from the table"));
    }
    ui->GB_updateClient->setDisabled(false);
}

void TabWidgetClients::printTicket()
{
    int idClient = ui->SB_idClientx->value();
    if(idClient != 0){

    QString CName,CDocN,DateDP,DateDV,pages,lang,phoneN,AdressLocal;


    this->DBH.transaction();
    QSqlQuery *query = new QSqlQuery(this->DBH);

    query->exec("SELECT fullname FROM clients WHERE id="+QString::number(idClient));
    if(query->next())
        CName = query->value(0).toString();

    query->exec("SELECT titleDoc,languageDoc,totalPages"
                ",depositeDay,deliveryDay FROM documents "
                "WHERE idClient="+QString::number(idClient));
    if(query->next()){
        CDocN  = query->value(0).toString();
        lang   = query->value(1).toString();
        pages  = query->value(2).toString();
        DateDP = query->value(3).toString();
        DateDV = query->value(4).toString();
    }

    query->exec("SELECT phoneNumber,localAdress FROM admin WHERE app_name='yassin' ");
    if(query->next()){
        phoneN = query->value(0).toString();
        AdressLocal = query->value(1).toString();
    }

    this->DBH.commit();

    QString Ticket = tr("N : ")+QString::number(idClient)+"\n"+
            tr("Full Name : ")+CName+"\n"+
            tr("Title : ")+CDocN+"\n"+
            "\n"+
            tr("Deposite day : ")+DateDP+"\n"+
            tr("Delivery day : ")+DateDV+"\n"+
            "\n\n";

    QPrinter printer;
    printer.setFullPage(true);
    printer.setPaperSize(QPrinter::A7);
    printer.setPageSize(QPrinter::A7);
    printer.setOrientation(QPrinter::Landscape);
    printer.setOutputFileName(QString::number(QDateTime::currentDateTime().toMSecsSinceEpoch())+"Ticket_"+QString::number(idClient)+".oxps");


    QPagedPaintDevice::Margins margs;
    margs.top = 1.0;
    margs.left = 2.0;
    margs.right = 2.0;
    margs.bottom=1.0;
    printer.setMargins(margs);

        QPrintDialog *dialog = new QPrintDialog(&printer);

        dialog->setWindowTitle(tr("Print Your Ticket"));

        if (dialog->exec() == QDialog::Accepted)
        {
            QPainter *painter = new QPainter();
            if(painter->begin(&printer))
            {
            painter->drawPixmap(12,12,60,60,QPixmap(":Icons/ic_local_library_black_48dp.png"));
            painter->setPen(Qt::black);
            painter->setFont(QFont("Arial", 12));
            painter->drawPixmap(345,210,22,22,QPixmap(":Icons/ic_domain_2x.png"));
            painter->drawText(-63, 210, 385, 35,  Qt::AlignRight , AdressLocal);
            painter->drawPixmap(345,235,22,22,QPixmap(":Icons/ic_call_2x.png"));
            painter->drawText(-63, 235, 385, 35,  Qt::AlignRight , phoneN);

            painter->setPen(Qt::black);
            painter->setFont(QFont("Arial", 17));
            painter->drawText(150, 9, 385, 270,  Qt::AlignTop , tr("library yassine"));

            painter->drawLine(80,45,380,45);
            painter->drawLine(80,46,380,46);

            painter->drawLine(10,200,380,200);
            painter->drawLine(10,201,380,201);


            painter->setPen(Qt::black);
            painter->setFont(QFont("Arial", 14));

            painter->drawRoundRect(0,0,385,265);
            painter->drawRoundRect(1,1,384,264);

            painter->drawText(-60, 48, 385, 265, Qt::AlignRight|Qt::AlignTop , Ticket);

            painter->end();
            }else{
                Toast *mToast = new Toast(this);
                mToast->setMessage(tr("Error repeat printing ticket"));
            }

        }
    }else{
        Toast *mToast = new Toast(this);
        mToast->setMessage(tr("select Client you want to print his ticket"));
    }

}
