#ifndef TABWIDGETCLIENTS_H
#define TABWIDGETCLIENTS_H

#include <QtWidgets/QTabWidget>

#include <QSqlQueryModel>
#include <QSqlQuery>
#include <QSqlTableModel>
#include <QSqlRelationalTableModel>
#include <QSortFilterProxyModel>
#include <QtWidgets/QDataWidgetMapper>

#include <QtWidgets/QComboBox>
#include <QtWidgets/QSpinBox>
#include <QPrinter>
#include <QPainter>
#include <QPrintDialog>

#include <QCalendarWidget>
#include <QHash>

#include <classes.h>
#include "dbh.h"
Q_DECLARE_LOGGING_CATEGORY(CLN)

namespace Ui {
class TabWidgetClients;
}

class TabWidgetClients : public QTabWidget
{
    Q_OBJECT
public:
    explicit TabWidgetClients(QWidget *parent = 0);
    Client *getDatafromForm();
    bool addClient(Client *data);
    bool verificationSubmitedData(Client *mClientData);

    ~TabWidgetClients();
    void displayOneRow();
    void clearFormUpdate();

public slots:
    void clearForm();
    void saveDemande2DB();
    void createMapper();
    void deleteClient();
    void printTicket();
    void initCalendar();
    void updateMinDeliveryDay(const QDate &date);
    void updateClient();

    void restPrice();
    void restPricex();
    void setmModelIndex(QModelIndex idx, QModelIndex idx2);
signals:
    void dataClientsChanged();

private:
    Ui::TabWidgetClients *ui;
    DBH *DB;

    QSqlTableModel    *modelClient;
    QSqlQueryModel    *queryModelClient;
    QDataWidgetMapper *mapper;
    QSortFilterProxyModel *proxyModelClient;
    QWidget *father;
    QHash<QString,int> TA;

};

#endif
