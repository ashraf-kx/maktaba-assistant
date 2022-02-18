#include "tabwidgetdocuments.h"
#include "ui_tabwidgetdocuments.h"
#include <QGraphicsDropShadowEffect>
#include <QDir>
#include <QCalendarWidget>

TabWidgetDocuments::TabWidgetDocuments(QWidget *parent) : QTabWidget(parent),
                                                          ui(new Ui::TabWidgetDocuments)
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

    mapper = new QDataWidgetMapper();

    QSqlDatabase connection = QSqlDatabase::database();
    qInfo() << "document connection is open: " << connection.open();
    modelDocs = new QSqlTableModel(this, connection);
    modelDocs->setTable("documents");
    modelDocs->select();

    QStringList listHeaders;
    listHeaders << tr("Title Doc");
    listHeaders << tr("Type Doc");
    listHeaders << tr("font Family");
    listHeaders << tr("font Size Doc");
    listHeaders << tr("Is Printed");
    listHeaders << tr("Language Doc");
    listHeaders << tr("Total Pages");
    listHeaders << tr("Pages Done");
    listHeaders << tr("Delivery Date");

    modelDocs->setHeaderData(0, Qt::Horizontal, tr("N#"));
    modelDocs->setHeaderData(3, Qt::Horizontal, listHeaders.at(0));
    modelDocs->setHeaderData(4, Qt::Horizontal, listHeaders.at(1));
    modelDocs->setHeaderData(5, Qt::Horizontal, listHeaders.at(2));
    modelDocs->setHeaderData(6, Qt::Horizontal, listHeaders.at(3));
    modelDocs->setHeaderData(8, Qt::Horizontal, listHeaders.at(4));
    modelDocs->setHeaderData(9, Qt::Horizontal, listHeaders.at(5));
    modelDocs->setHeaderData(10, Qt::Horizontal, listHeaders.at(6));
    modelDocs->setHeaderData(11, Qt::Horizontal, listHeaders.at(7));
    modelDocs->setHeaderData(13, Qt::Horizontal, listHeaders.at(8));

    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);

    ui->tableView->setSortingEnabled(true);
    ui->tableView->setSelectionMode(QAbstractItemView::SingleSelection);

    proxyModelDocs = new QSortFilterProxyModel(this);

    proxyModelDocs->setSourceModel(modelDocs);

    proxyModelDocs->setFilterRegExp(QRegExp("", Qt::CaseInsensitive, QRegExp::FixedString));
    proxyModelDocs->setFilterKeyColumn(3);

    ui->tableView->setModel(proxyModelDocs);
    ui->tableView->horizontalHeader()->stretchLastSection();
    ui->tableView->verticalHeader()->stretchLastSection();
    ui->tableView->resizeColumnsToContents();
    ui->tableView->show();

    ui->tableView->setColumnHidden(1, true);
    ui->tableView->setColumnHidden(2, true);
    ui->tableView->setColumnHidden(5, false);
    ui->tableView->setColumnHidden(6, false);
    ui->tableView->setColumnHidden(7, true);
    ui->tableView->setColumnHidden(12, true);
    ui->tableView->setColumnHidden(14, true);
    ui->tableView->setColumnHidden(15, true);
    ui->tableView->setColumnHidden(16, true);
    ui->tableView->setColumnHidden(17, true);
    ui->tableView->setColumnHidden(18, true);
    ui->tableView->setColumnHidden(19, true);

    startMapper();

    sh = new QGraphicsDropShadowEffect();
    sh->setBlurRadius(8);
    sh->setOffset(2);
    sh->setColor(QColor(63, 63, 63, 180));

    QGraphicsDropShadowEffect *shX = new QGraphicsDropShadowEffect();
    shX->setBlurRadius(8);
    shX->setOffset(2);
    shX->setColor(QColor(63, 63, 63, 180));

    mGridLayout = new QGridLayout(ui->groupBox);
    mGridLayout->setHorizontalSpacing(10);

    mGridLayout2 = new QGridLayout(ui->groupBox_);
    mGridLayout2->setHorizontalSpacing(10);

    mapper->setSubmitPolicy(mapper->ManualSubmit);

    updateLists();
    currentActiveStats();
    displayArchivedDocs();
    clearFormUpdate();

    ui->SB_idDocx->setVisible(false);
    ui->SB_idDocx->setValue(0);

    ui->BT_associate->setGraphicsEffect(sh);
    ui->BT_updateDoc->setGraphicsEffect(sh1);
    ui->widgetSearch->setGraphicsEffect(sh2);

    connect(ui->LE_fullnameFiltre, SIGNAL(textChanged(QString)), proxyModelDocs, SLOT(setFilterRegExp(QString)));
    connect(ui->BT_associate, SIGNAL(clicked(bool)), ui->BT_associate, SLOT(setDisabled(bool)));
    connect(ui->BT_associate, SIGNAL(clicked()), this, SLOT(doAssociate()));

    connect(ui->tableView->selectionModel(), SIGNAL(currentRowChanged(QModelIndex, QModelIndex)),
            mapper, SLOT(setCurrentModelIndex(QModelIndex)));

    connect(ui->BT_updateDoc, SIGNAL(pressed()), this, SLOT(updateDoc()));
    connect(ui->BT_updateDoc, SIGNAL(released()), this, SLOT(globaleUpdate()));

    connect(ui->SB_pagesDonex, SIGNAL(valueChanged(int)), this, SLOT(calculatePagesRest()));
    connect(ui->SB_totalPagesx, SIGNAL(valueChanged(int)), this, SLOT(calculatePagesRest()));
    connect(ui->SB_idDocx, SIGNAL(valueChanged(int)), this, SLOT(displayNbrDoc(int)));
}

void TabWidgetDocuments::updateDoc()
{
    if (ui->SB_pagesDonex->value() <= ui->SB_totalPagesx->value())
    {
        if ((ui->SB_pagesWordx->value() + ui->SB_pagesHandx->value()) <= ui->SB_totalPagesx->value())
        {
            QSqlDatabase connection = QSqlDatabase::database("mainConnection");
            if (connection.open())
            {
                connection.transaction();

                //! [1] Save data into workers table.
                QSqlQuery *query = new QSqlQuery(connection);

                query->prepare("UPDATE documents SET "
                               "pagesDone= :pagesDone, pagesHand= :pagesHand, pagesWord= :pagesWord,"
                               "isPrinted= :isPrinted, deliveryDay= :deliveryDay WHERE "
                               "id=:id");

                query->bindValue(":id", ui->SB_idDocx->value());
                query->bindValue(":pagesDone", ui->SB_pagesDonex->value());
                query->bindValue(":pagesHand", ui->SB_pagesHandx->value());
                query->bindValue(":pagesWord", ui->SB_pagesWordx->value());
                query->bindValue(":isPrinted", ui->CB_isPrintedx->currentText());
                query->bindValue(":deliveryDay", ui->DE_deliveryDayx->date().toString("yyyy-MM-dd"));

                query->exec();

                connection.commit();

                modelDocs->select();
                clearFormUpdate();

                connection.close();
            }
        }
        else
        {
            Toast *mToast = new Toast(this);
            mToast->setMessage(tr("error value pages Hand plus word more then total pages "));
        }
    }
    else
    {
        Toast *mToast = new Toast(this);
        mToast->setMessage(tr("error value pages done more then total pages "));
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

QList<TabWidgetDocuments::Document> TabWidgetDocuments::getListDocsAvailable()
{
    QList<Document> list;
    Document temp;
    list.clear();

    QSqlDatabase connection = QSqlDatabase::database();
    if (connection.open())
    {
        connection.transaction();
        //! [1] Save data into workers table.
        QSqlQuery *query = new QSqlQuery(connection);
        QSqlQuery *subQuery;
        query->prepare("SELECT  id,idClient,titleDoc FROM documents WHERE dateStarted = '-1' ORDER BY titleDoc ASC ");
        query->exec();
        while (query->next())
        {
            temp.id = query->value(0).toInt();
            int idOwner = query->value(1).toInt();
            temp.title = query->value(2).toString();

            subQuery = new QSqlQuery(connection);
            subQuery->prepare("SELECT fullname FROM clients WHERE id='" + QString::number(idOwner) + "' ");
            subQuery->exec();
            while (subQuery->next())
                temp.owner = subQuery->value(0).toString();
            list << temp;
            subQuery->clear();
        }
        connection.commit();
        connection.close();
    }

    return list;
}

QList<TabWidgetDocuments::Employer> TabWidgetDocuments::getListWorkers()
{
    QList<Employer> list;
    Employer temp;
    list.clear();

    QSqlDatabase connection = QSqlDatabase::database();
    if (connection.open())
    {
        connection.transaction();
        QSqlQuery *query = new QSqlQuery(connection);
        query->prepare("SELECT id,fullName FROM workers WHERE isDisponible= :isDisponible ");
        query->bindValue(":isDisponible", tr("worker disponible"));
        query->exec();

        while (query->next())
        {
            temp.id = query->value(0).toInt();
            temp.fullName = query->value(1).toString();
            list << temp;
        }
        query->clear();

        connection.commit();
        connection.close();
    }
    return list;
}

void TabWidgetDocuments::updateLists()
{
    listdocs = getListDocsAvailable();
    ui->CB_pickDoc->clear();
    for (int i = 0; i < listdocs.size(); i++)
        ui->CB_pickDoc->addItem((listdocs.at(i)).title);
    ui->CB_pickDoc->update();

    listEmpls = getListWorkers();
    ui->CB_pickEmpl->clear();
    for (int i = 0; i < listEmpls.size(); i++)
        ui->CB_pickEmpl->addItem((listEmpls.at(i)).fullName);
    ui->CB_pickEmpl->update();

    ui->BT_associate->setEnabled(true);
}

void TabWidgetDocuments::doAssociate()
{
    if (listdocs.size() > 0)
    {
        int indexWorker = 0;
        int indexDoc = 0;
        QString mTitleDoc = ui->CB_pickDoc->currentText();
        QString mNameWorker = ui->CB_pickEmpl->currentText();

        QSqlDatabase connection = QSqlDatabase::database();
        if (connection.open())
        {
            connection.transaction();
            //! look for indexDoc & indexWorker.
            QSqlQuery *query = new QSqlQuery(connection);
            query->prepare("SELECT id FROM workers WHERE fullName=:fullName ");
            query->bindValue(":fullName", mNameWorker);
            query->exec();

            while (query->next())
                indexWorker = query->value(0).toInt();

            query->clear();
            query->prepare("SELECT id FROM documents WHERE titleDoc=:titleDoc");
            query->bindValue(":titleDoc", mTitleDoc);
            query->exec();

            while (query->next())
                indexDoc = query->value(0).toInt();

            //! [1] update necessery data in table "workers/documents".
            query->clear();
            query->exec("UPDATE documents SET pagesDone=0,dateStarted='" + QDate::currentDate().toString("yyyy-MM-dd") + "', idWorker='" + QString::number(indexWorker) + "' WHERE id=" + QString::number(indexDoc) + " ");
            query->exec("UPDATE workers SET currentDocID='" + QString::number(indexDoc) + "' WHERE id=" + QString::number(indexWorker) + " ");

            connection.commit();

            modelDocs->select();
            ui->tableView->update();

            connection.close();
        }

        updateLists();
        currentActiveStats();
        displayArchivedDocs();
    }
    ui->BT_associate->setEnabled(true);
}

void TabWidgetDocuments::currentActiveStats()
{
    QSqlDatabase connection = QSqlDatabase::database("mainConnection");
    if (connection.open())
    {
        connection.transaction();

        foreach (ActiveDoc *form, ui->groupBox->findChildren<ActiveDoc *>())
            delete form;

        //! [1] update necessery data in table "workers/documents".
        QSqlQuery *query = new QSqlQuery(connection);
        QSqlQuery *subQuery;
        query->prepare("SELECT id,idWorker,titleDoc,totalPages,pagesDone,depositeDay,deliveryDay FROM documents WHERE dateStarted<>-1 AND dateFinished=-1");
        query->exec();

        int row = 0;
        int col = 0;
        while (query->next())
        {
            //! create and place Items in the groupBox.
            int idDoc = query->value(0).toInt();
            int idWorker = query->value(1).toInt();
            QString nameWorker = "";
            subQuery = new QSqlQuery(connection);
            subQuery->prepare("SELECT fullName FROM workers WHERE id=" + QString::number(idWorker));
            subQuery->exec();
            while (subQuery->next())
                nameWorker = subQuery->value(0).toString();

            QString titleDoc = query->value(2).toString();
            int totalPages = query->value(3).toInt();
            int pagesDone = query->value(4).toInt();
            QDate depositeDay = QDate::fromString(query->value(5).toString(), "yyyy-MM-dd");
            QDate deliveryDay = QDate::fromString(query->value(6).toString(), "yyyy-MM-dd");

            ActiveDoc *form = new ActiveDoc(this);
            form->_ActiveDoc(idDoc, nameWorker, idWorker, titleDoc, totalPages, pagesDone, depositeDay, deliveryDay);

            mGridLayout->addWidget(form, row, col);
            col++;
            if (col == 3)
            {
                row++;
                col = 0;
            }

            form->setVisible(true);
            subQuery->clear();
        }

        query->clear();
        connection.commit();

        this->modelDocs->select();
        ui->tableView->update();

        connection.close();
    }
}

void TabWidgetDocuments::displayArchivedDocs()
{
    QSqlDatabase connection = QSqlDatabase::database();
    if (connection.open())
    {
        connection.transaction();

        foreach (ArchivedDoc *form, ui->groupBox_->findChildren<ArchivedDoc *>())
            delete form;

        QSqlQuery *query = new QSqlQuery(connection);
        QSqlQuery *subQuery;
        query->prepare("SELECT id,idClient,titleDoc,pagesDone FROM documents WHERE dateFinished<>-1");
        query->exec();

        int row = 0;
        int col = 0;
        while (query->next())
        {
            //! create and place Items in the groupBox.
            QString nameClient = "";
            int idDoc = query->value(0).toInt();
            subQuery = new QSqlQuery(connection);
            subQuery->prepare("SELECT fullname FROM clients WHERE id=" + QString::number(query->value(1).toInt()));
            subQuery->exec();

            while (subQuery->next())
                nameClient = subQuery->value(0).toString();

            QString titleDoc = query->value(2).toString();
            int pagesDone = query->value(3).toInt();

            ArchivedDoc *form = new ArchivedDoc(this);
            form->showArchivedDocs(idDoc, nameClient, titleDoc, pagesDone);

            mGridLayout2->addWidget(form, row, col);
            col++;
            if (col == 3)
            {
                row++;
                col = 0;
            }

            form->setVisible(true);
            subQuery->clear();
        }
        query->clear();

        connection.commit();

        this->modelDocs->select();
        ui->tableView->update();

        connection.close();
    }
}

void TabWidgetDocuments::startMapper()
{
    mapper->setModel(proxyModelDocs);

    mapper->addMapping(ui->SB_idDocx, modelDocs->fieldIndex("id"));

    mapper->addMapping(ui->DE_deliveryDayx, modelDocs->fieldIndex("deliveryDay"));
    ui->DE_deliveryDayx->setMinimumDate(QDate::currentDate());
    ui->DE_deliveryDayx->calendarWidget()->setFirstDayOfWeek(Qt::Saturday);
    ui->DE_deliveryDayx->calendarWidget()->setGridVisible(true);
    ui->DE_deliveryDayx->calendarWidget()->setHorizontalHeaderFormat(QCalendarWidget::SingleLetterDayNames);
    ui->DE_deliveryDayx->calendarWidget()->setVerticalHeaderFormat(QCalendarWidget::NoVerticalHeader);
    ui->DE_deliveryDayx->calendarWidget()->setLocale(QLocale(QLocale::Arabic, QLocale::Algeria));

    mapper->addMapping(ui->SB_pagesDonex, modelDocs->fieldIndex("pagesDone"));
    mapper->addMapping(ui->SB_totalPagesx, modelDocs->fieldIndex("totalPages"));
    mapper->addMapping(ui->CB_isPrintedx, modelDocs->fieldIndex("isPrinted"));
    mapper->addMapping(ui->SB_pagesHandx, modelDocs->fieldIndex("pagesHand"));
    mapper->addMapping(ui->SB_pagesWordx, modelDocs->fieldIndex("pagesWord"));
}

void TabWidgetDocuments::calculatePagesRest()
{
    if (ui->SB_pagesDonex->value() <= ui->SB_totalPagesx->value())
    {
        ui->Lb_pagesRestx->setText(QString::number(ui->SB_totalPagesx->value() - ui->SB_pagesDonex->value()));
    }
    else
    {
        ui->Lb_pagesRestx->setText("*");
    }
}

void TabWidgetDocuments::displayNbrDoc(int val)
{
    if (val == 0)
        ui->LB_idDocx->setText("*");
    else
        ui->LB_idDocx->setText(QString::number(val));
}

TabWidgetDocuments::~TabWidgetDocuments()
{
    delete ui;
}
