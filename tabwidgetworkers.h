#ifndef TABWIDGETWORKERS_H
#define TABWIDGETWORKERS_H

#include <QtWidgets/QTabWidget>
#include <QSqlQueryModel>
#include <QSqlQuery>
#include <QSqlTableModel>
#include <QSqlRelationalTableModel>
#include <QSortFilterProxyModel>
#include <QtWidgets/QDataWidgetMapper>
#include <QDateTime>
#include "dialog.h"
#include "toast.h"

#include <classes.h>
#include "dbh.h"
Q_DECLARE_LOGGING_CATEGORY(WRK)

namespace Ui {
class TabWidgetWorkers;
}

class TabWidgetWorkers : public QTabWidget
{
    Q_OBJECT
    
public:
    explicit TabWidgetWorkers(QWidget *parent = 0);
    Worker* getDatafromForm();
    bool addWorker(Worker* data);
    bool verificationSubmitedData(Worker* data);
    ~TabWidgetWorkers();

    void clearFormUpdate();

public slots:
    void clearForm();
    void saveDemande2DB();
    void createMapper();
    void deleteWorker();
    void updateWorker();

    void changeIconDisponibility();
signals:
    void dataWorkersChanged();

private:
    Ui::TabWidgetWorkers *ui;
    DBH *DB;

    QSqlQueryModel        *queryModelWorker;
    QSortFilterProxyModel *proxyModelWorker;
    QDataWidgetMapper     *mapper;
    QHash<QString,int> TA;
};

#endif // TABWIDGETWORKERS_H
