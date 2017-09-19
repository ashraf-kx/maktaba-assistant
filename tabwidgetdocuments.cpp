#include "tabwidgetdocuments.h"
#include "ui_tabwidgetdocuments.h"
#include <QGraphicsDropShadowEffect>
#include <QDir>
#include <QCalendarWidget>

Q_LOGGING_CATEGORY(DOC,"DOCUMENT")

TabWidgetDocuments::TabWidgetDocuments(QWidget *parent) :
    QTabWidget(parent),
    ui(new Ui::TabWidgetDocuments)
{
    ui->setupUi(this);

    DB             = new DBH("_documents_");

    mapper         = new QDataWidgetMapper();
    queryModelDocs = new QSqlQueryModel();
    queryModelDocs->setQuery("SELECT `id`,`titleDoc`,`typeDoc`,"
                             "`fontFamily`,`fontSize`,`printingColor`,"
                             "`isPrinted`,`languageDoc`,`totalPages`,`pagesDone`,"
                             "`pagesHand`,`pagesWord`,`deliveryDay` FROM `documents`",DB->getCnx());

    TA.clear();
    TA["id"]            = 0;
    TA["titleDoc"]      = 1;
    TA["typeDoc"]       = 2;
    TA["fontFamily"]    = 3;
    TA["fontSize"]      = 4;
    TA["printingColor"] = 5;
    TA["isPrinted"]     = 6;
    TA["languageDoc"]   = 7;
    TA["totalPages"]    = 8;
    TA["pagesDone"]     = 9;
    TA["pagesHand"]     = 10;
    TA["pagesWord"]     = 11;
    TA["deliveryDay"]   = 12;

    queryModelDocs->setHeaderData(TA["id"], Qt::Horizontal, tr("N#"));
    queryModelDocs->setHeaderData(TA["titleDoc"], Qt::Horizontal, tr("Title Doc"));
    queryModelDocs->setHeaderData(TA["typeDoc"], Qt::Horizontal, tr("Type Doc"));
    queryModelDocs->setHeaderData(TA["fontFamily"], Qt::Horizontal, tr("font Family"));
    queryModelDocs->setHeaderData(TA["fontSize"], Qt::Horizontal, tr("font Size Doc"));
    queryModelDocs->setHeaderData(TA["printingColor"], Qt::Horizontal, tr("Printing Color"));
    queryModelDocs->setHeaderData(TA["isPrinted"], Qt::Horizontal, tr("Is Printed"));
    queryModelDocs->setHeaderData(TA["languageDoc"], Qt::Horizontal, tr("Language Doc"));
    queryModelDocs->setHeaderData(TA["totalPages"], Qt::Horizontal, tr("Total Pages"));
    queryModelDocs->setHeaderData(TA["pagesDone"], Qt::Horizontal, tr("Pages Done"));
    queryModelDocs->setHeaderData(TA["pagesHand"], Qt::Horizontal, tr("Pages By Hand"));
    queryModelDocs->setHeaderData(TA["pagesWord"], Qt::Horizontal, tr("Pages By Word"));
    queryModelDocs->setHeaderData(TA["deliveryDay"], Qt::Horizontal, tr("Delivery Date"));

    proxyModelDocs = new QSortFilterProxyModel(this);
    proxyModelDocs->setSourceModel(queryModelDocs);
    ui->tableView->setModel(proxyModelDocs);

//    proxyModelDocs->setFilterRegExp(QRegExp("", Qt::CaseInsensitive,QRegExp::FixedString));
//    proxyModelDocs->setFilterKeyColumn(3);

    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableView->setSortingEnabled(true);
    ui->tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableView->horizontalHeader()->resizeSections(QHeaderView::ResizeToContents);
    ui->tableView->horizontalHeader()->stretchLastSection();
    ui->tableView->verticalHeader()->stretchLastSection();
    ui->tableView->resizeColumnsToContents();
    ui->tableView->resizeRowsToContents();
    ui->tableView->horizontalHeader()->setResizeContentsPrecision(0);
    ui->tableView->setWordWrap(true);
    ui->tableView->setVisible(true);

    mapper->setSubmitPolicy(mapper->ManualSubmit);
    createMapper();

    mGridLayout = new QGridLayout(ui->groupBox);
    mGridLayout->setHorizontalSpacing(10);

    mGridLayout2 = new QGridLayout(ui->groupBox_);
    mGridLayout2->setHorizontalSpacing(10);

    updateLists();
    currentActiveStats();
    displayArchivedDocs();
    clearFormUpdate();

    ui->SB_idDocx->setVisible(false);
    ui->SB_idDocx->setValue(0);

    ui->BT_associate->setGraphicsEffect(Style::shadowbutton());
    ui->BT_updateDoc->setGraphicsEffect(Style::shadowbutton());
    ui->widgetSearch->setGraphicsEffect(Style::shadowbutton());

    connect(ui->Le_search,SIGNAL(textChanged(QString)),proxyModelDocs,SLOT(setFilterRegExp(QString)));
    connect(ui->BT_associate,SIGNAL(clicked(bool)),ui->BT_associate,SLOT(setDisabled(bool)));
    connect(ui->BT_associate,SIGNAL(clicked()),this,SLOT(doAssociate()));

    connect(ui->tableView->selectionModel(), SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),
            mapper, SLOT(setCurrentModelIndex(QModelIndex)));

    connect(ui->BT_updateDoc,SIGNAL(pressed()),this,SLOT(updateDoc()));
    connect(ui->BT_updateDoc,SIGNAL(released()),this,SLOT(globaleUpdate()));

    connect(ui->SB_pagesDonex,SIGNAL(valueChanged(int)),this,SLOT(calculatePagesRest()));
    connect(ui->SB_totalPagesx,SIGNAL(valueChanged(int)),this,SLOT(calculatePagesRest()));
    connect(ui->SB_idDocx,SIGNAL(valueChanged(int)),this,SLOT(displayNbrDoc(int)));

    // Table View Data Contolled.
    connect(ui->Bt_edit,SIGNAL(pressed()),ui->widget,SLOT(hide()));
    connect(ui->Bt_edit,SIGNAL(released()),ui->widget,SLOT(show()));

    initListRowsPerPage();
    ui->Cb_rows->setCurrentIndex(0);

    connect(ui->Cb_rows,SIGNAL(currentTextChanged(QString)),this,SLOT(updateTableViewRows()));
    connect(ui->Cb_pages,SIGNAL(currentTextChanged(QString)),this,SLOT(showPageRows()));

    connect(ui->Le_search,SIGNAL(textChanged(QString)),
            proxyModelDocs,SLOT(setFilterRegExp(QString)));

    connect(ui->Bt_previous,SIGNAL(clicked(bool)),this,SLOT(previousPage()));
    connect(ui->Bt_next,SIGNAL(clicked(bool)),this,SLOT(nextPage()));
}

void TabWidgetDocuments::updateDoc()
{
    if(ui->SB_pagesDonex->value() <= ui->SB_totalPagesx->value())
    {
        if((ui->SB_pagesWordx->value()+ui->SB_pagesHandx->value()) <= ui->SB_totalPagesx->value() )
        {
            Document* doc = new Document();

            doc->setID(ui->SB_idDocx->value());
            doc->setPagesDone(ui->SB_pagesDonex->value());
            doc->setPagesHand(ui->SB_pagesHandx->value());
            doc->setPagesWord(ui->SB_pagesWordx->value());
            doc->setIsPrinted(ui->CB_isPrintedx->currentText().compare(tr("unprinted")));
            doc->setDeliveryDay(ui->DE_deliveryDayx->date().toString("yyyy-MM-dd"));

            DB->updateDocument(doc);

            queryModelDocs->setQuery("SELECT `id`,`titleDoc`,`typeDoc`,"
                                     "`fontFamily`,`fontSize`,`printingColor`,"
                                     "`isPrinted`,`languageDoc`,`totalPages`,`pagesDone`,"
                                     "`pagesHand`,`pagesWord`,`deliveryDay` FROM `documents`",DB->getCnx());
            clearFormUpdate();
        }else
        {
            Toast *mToast = new Toast(this);
            mToast->show(tr("error value pages Hand plus word more then total pages "),"");
        }
    }else{
        Toast *mToast = new Toast(this);
        mToast->show(tr("error value pages done more then total pages "),"");
    }
}

void TabWidgetDocuments::clearFormUpdate()
{
    ui->SB_idDocx->setValue(0);
    ui->SB_pagesDonex->setValue(0);
    ui->SB_totalPagesx->setValue(0);
    ui->SB_pagesHandx->setValue(0);
    ui->SB_pagesWordx->setValue(0);
    ui->DE_deliveryDayx->setDate(QDate::currentDate());
}

void TabWidgetDocuments::globaleUpdate()
{
    updateLists();
    currentActiveStats();
    displayArchivedDocs();
}

void TabWidgetDocuments::updateLists()
{
    docsList  = DB->getAvailableDocsList();

    ui->CB_pickDoc->clear();
    for(int i=0;i<docsList.size();i++)
        ui->CB_pickDoc->addItem((docsList.at(i))->getTitleDoc());
    ui->CB_pickDoc->update();

    workersList = DB->getWorkersList();
    ui->CB_pickWrkr->clear();
    for(int i=0;i<workersList.size();i++)
        ui->CB_pickWrkr->addItem((workersList.at(i))->getFullName());
    ui->CB_pickWrkr->update();

    ui->BT_associate->setEnabled(true);
}

void TabWidgetDocuments::doAssociate()
{
    if(docsList.size()>0)
    {
        DB->assignDoc2Worker(ui->CB_pickWrkr->currentText(),ui->CB_pickDoc->currentText());
        queryModelDocs->setQuery("SELECT `id`,`titleDoc`,`typeDoc`,"
                                 "`fontFamily`,`fontSize`,`printingColor`,"
                                 "`isPrinted`,`languageDoc`,`totalPages`,`pagesDone`,"
                                 "`pagesHand`,`pagesWord`,`deliveryDay` FROM `documents`",DB->getCnx()); // TODO : Change Me ASSHOLE.
        // update ListDoc . but ListWorkers nothing changes.
        updateLists();
        currentActiveStats();
        displayArchivedDocs();
    }
    ui->BT_associate->setEnabled(true);
}

void TabWidgetDocuments::currentActiveStats()
{
    foreach (ActiveDoc *form, ui->groupBox->findChildren<ActiveDoc*>())
       delete form;

    QList<Document*> docsList = DB->getActiveDocsList();

    int row=0; int col=0;
    foreach (Document *doc, docsList) {

        QString workerName= DB->getWorkerNameByID(doc->getWriterID());

        ActiveDoc *form= new ActiveDoc(this);
        form->_ActiveDoc(doc->getID(),
                         workerName,
                         doc->getWriterID(),
                         doc->getTitleDoc(),
                         doc->getTotalPages(),
                         doc->getPagesDone(),
                         QDate::fromString(doc->getDepositeDay(),"yyyy-MM-dd"),
                         QDate::fromString(doc->getDeliveryDay(),"yyyy-MM-dd"));
        mGridLayout->addWidget(form,row,col);
        col++;
        if(col == 3){ row++; col=0; }
        form->setVisible(true);
    }

    queryModelDocs->setQuery("SELECT `id`,`titleDoc`,`typeDoc`,"
                             "`fontFamily`,`fontSize`,`printingColor`,"
                             "`isPrinted`,`languageDoc`,`totalPages`,`pagesDone`,"
                             "`pagesHand`,`pagesWord`,`deliveryDay` FROM `documents`",DB->getCnx());
}

void TabWidgetDocuments::displayArchivedDocs()
{
    foreach (ArchivedDoc *form, ui->groupBox_->findChildren<ArchivedDoc*>())
       delete form;

    QList<Document*> docsList  = DB->getArchivedDocsList();

    int row = 0;  int col = 0;
    foreach (Document* doc, docsList) {

        DB->getClientNameByID(doc->getOwnerID());

        ArchivedDoc *form= new ArchivedDoc(this);
        form->showArchivedDoc(doc->getID());

        mGridLayout2->addWidget(form,row,col);
        col++;
        if(col == 3){ row++; col=0; }

        form->setVisible(true);
    }

    queryModelDocs->setQuery("SELECT `id`,`titleDoc`,`typeDoc`,"
                             "`fontFamily`,`fontSize`,`printingColor`,"
                             "`isPrinted`,`languageDoc`,`totalPages`,`pagesDone`,"
                             "`pagesHand`,`pagesWord`,`deliveryDay` FROM `documents`",DB->getCnx());
}

void TabWidgetDocuments::createMapper()
{
    mapper->setModel(proxyModelDocs);

    mapper->addMapping(ui->SB_idDocx,TA["id"]);
    mapper->addMapping(ui->DE_deliveryDayx, TA["deliveryDay"]);

    ui->DE_deliveryDayx->setMinimumDate(QDate::currentDate());
    ui->DE_deliveryDayx->calendarWidget()->setFirstDayOfWeek(Qt::Saturday);
    ui->DE_deliveryDayx->calendarWidget()->setGridVisible(true);
    ui->DE_deliveryDayx->calendarWidget()->setHorizontalHeaderFormat(QCalendarWidget::SingleLetterDayNames);
    ui->DE_deliveryDayx->calendarWidget()->setVerticalHeaderFormat(QCalendarWidget::NoVerticalHeader);
    ui->DE_deliveryDayx->calendarWidget()->setLocale(QLocale(QLocale::Arabic,QLocale::Algeria));

    mapper->addMapping(ui->SB_pagesDonex, TA["pagesDone"]);
    mapper->addMapping(ui->SB_totalPagesx,TA["totalPages"]);
    mapper->addMapping(ui->CB_isPrintedx, TA["isPrinted"]);
    mapper->addMapping(ui->SB_pagesHandx, TA["pagesHand"]);
    mapper->addMapping(ui->SB_pagesWordx, TA["pagesWord"]);
}

void TabWidgetDocuments::calculatePagesRest()
{
    if(ui->SB_pagesDonex->value() <= ui->SB_totalPagesx->value())
    {
        ui->Lb_pagesRestx->setText(QString::number(ui->SB_totalPagesx->value()-ui->SB_pagesDonex->value()));
    }
    else
    {
        ui->Lb_pagesRestx->setText("*");
    }
}

void TabWidgetDocuments::displayNbrDoc(int val)
{
    if(val == 0) ui->LB_idDocx->setText("*");
    else ui->LB_idDocx->setText(QString::number(val));
}

TabWidgetDocuments::~TabWidgetDocuments()
{
    delete ui;
}


// Controle the table View Data, show only 15 line and so on .
void TabWidgetDocuments::setmModelIndex(QModelIndex idx,QModelIndex idx2)
{
    Q_UNUSED(idx2);
    mapper->setCurrentModelIndex(idx);
}

void TabWidgetDocuments::updateMessageInfo()
{
    int startIdx =0;
    int numRowsVisible =0;
    int modelRows = queryModelDocs->rowCount();
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

void TabWidgetDocuments::nextPage()
{
    int idxCurrent = ui->Cb_pages->currentIndex();
    if(idxCurrent < ui->Cb_pages->count()-1 )
    {
        ui->Cb_pages->setCurrentIndex(idxCurrent+1);
        updateMessageInfo();
    }
}

void TabWidgetDocuments::previousPage()
{
    int idxCurrent = ui->Cb_pages->currentIndex();
    if(idxCurrent > 0 )
    {
        ui->Cb_pages->setCurrentIndex(idxCurrent-1);
        updateMessageInfo();
    }
}

void TabWidgetDocuments::selectedColumn(QModelIndex idx,QModelIndex idx2)
{
    Q_UNUSED(idx2)
    qDebug()<<"row : "<<idx.row()<<"Collumn : "<<idx.column();

   idxColSelected = idx.column();//ui->tableView->currentIndex().column();
   if(idxColSelected >= 0 )
   {
       proxyModelDocs->setFilterRegExp(QRegExp("", Qt::CaseInsensitive));
       proxyModelDocs->setFilterKeyColumn(idxColSelected);
       ui->Le_search->setPlaceholderText(tr("Search By ")+queryModelDocs->headerData(idxColSelected,Qt::Horizontal,Qt::DisplayRole).toString());
   }
}

void TabWidgetDocuments::hideAllRows()
{
    for(int i=0; i<queryModelDocs->rowCount(); i++)
    {
        ui->tableView->hideRow(i);
    }
}

void TabWidgetDocuments::showAllRows()
{
    for(int i=0; queryModelDocs->rowCount();i++)
    {
        ui->tableView->showRow(i);
    }
}

void TabWidgetDocuments::initListRowsPerPage()
{
    ui->Cb_rows->clear();
    int maxRows  = queryModelDocs->rowCount();
    ui->Cb_rows->addItem(QString::number(maxRows));

    if(maxRows >= 10 ) ui->Cb_rows->addItem("10");
    if(maxRows >= 15 ) ui->Cb_rows->addItem("15");
    if(maxRows >= 20 ) ui->Cb_rows->addItem("20");
    if(maxRows >= 30 ) ui->Cb_rows->addItem("30");
    if(maxRows >= 50 ) ui->Cb_rows->addItem("50");
    if(maxRows >= 100 ) ui->Cb_rows->addItem("100");
}

void TabWidgetDocuments::initListNumberPages()
{
    ui->Cb_pages->clear();
    int maxRows   = queryModelDocs->rowCount();
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

void TabWidgetDocuments::showPageRows()
{
    hideAllRows();
    int startIdx =0;
    int numRowsVisible =0;
    if(ui->Cb_pages->count()>0 && ui->Cb_rows->count()>0)
    {
        startIdx = (ui->Cb_pages->currentText().toInt()-1)*ui->Cb_rows->currentText().toInt();
        numRowsVisible = ui->Cb_rows->currentText().toInt();

        int endIdx = startIdx+numRowsVisible;
        if(endIdx>queryModelDocs->rowCount()) endIdx = queryModelDocs->rowCount();

        for(int i = startIdx; i< endIdx; i++)
        {
            ui->tableView->showRow(i);
        }
        updateMessageInfo();
    }
}

void TabWidgetDocuments::updateTableViewRows()
{
    initListNumberPages();
    showPageRows();
}
