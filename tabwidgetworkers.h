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

namespace Ui {
class TabWidgetWorkers;
}

class TabWidgetWorkers : public QTabWidget
{
    Q_OBJECT
    struct workerData {
        QString fullName;
        QString phoneNumber;
        QString email;
        QString remarke;
        QString isDisponible;
        int currnetDocID;
        int lastDocID;
        int nextDocID;
        QList<int> listDocsHundled;
        uint date_created;
        uint date_modified;
    };
    
public:
    explicit TabWidgetWorkers(QWidget *parent = 0);
    workerData getDatafromForm();
    bool addWorker(workerData mWorkerData);
    bool verificationSubmitedData(workerData mWorkerData);
    ~TabWidgetWorkers();

    void clearFormUpdate();

public slots:
    void clearForm();
    void saveDemande2DB();
    void startMapper();
    void deleteWorker();
    void updateWorker();

    void changeIconDisponibility();
signals:
    void dataWorkersChanged();

private:
    Ui::TabWidgetWorkers *ui;
    QSqlDatabase          DBH;

    QSqlTableModel        *modelWorker;
    QSortFilterProxyModel *proxyModelWorker;
    QDataWidgetMapper     *mapper;

};

#endif // TABWIDGETWORKERS_H
