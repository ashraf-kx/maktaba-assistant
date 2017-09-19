#ifndef TABWIDGETDOCUMENTS_H
#define TABWIDGETDOCUMENTS_H

#include <QtWidgets/QTabWidget>

#include <QSqlQueryModel>
#include <QSqlQuery>
#include <QSqlTableModel>
#include <QSqlRelationalTableModel>
#include <QSortFilterProxyModel>
#include <QtWidgets/QDataWidgetMapper>

#include <QDebug>
#include <QDate>
#include <QtWidgets/QGraphicsDropShadowEffect>
#include <QtWidgets/QGridLayout>

#include "archiveddoc.h"
#include "activedoc.h"
#include "toast.h"
#include <QHash>

#include <classes.h>
#include "dbh.h"
Q_DECLARE_LOGGING_CATEGORY(DOC)

namespace Ui {
class TabWidgetDocuments;
}

class TabWidgetDocuments : public QTabWidget
{
    Q_OBJECT
    
public:
    explicit TabWidgetDocuments(QWidget *parent = 0);
    QList<Document> getListDocsAvailable();
    QList<Worker *> getWorkersList();
    void createMapper();
    void clearFormUpdate();

    ~TabWidgetDocuments();

public slots:
    void updateLists();
    void doAssociate();
    void currentActiveStats();
    void displayArchivedDocs();
    void globaleUpdate();
    void updateDoc();

    void calculatePagesRest();
    void displayNbrDoc(int val);

private:
    Ui::TabWidgetDocuments *ui;
    DBH *DB;
    QSqlQueryModel    *queryModelDocs;
    QDataWidgetMapper *mapper;
    QSortFilterProxyModel *proxyModelDocs;
    QList<Document*> docsList;
    QList<Worker*>   workersList;

    QSqlTableModel *modelClient;
    QGraphicsDropShadowEffect *sh;
    QGridLayout *mGridLayout;
    QGridLayout *mGridLayout2;
    QHash<QString,int> TA;
};

#endif // TABWIDGETDOCUMENTS_H
