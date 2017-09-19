#ifndef ACTIVEDOC_H
#define ACTIVEDOC_H

#include <QtWidgets/QWidget>

#include "ui_activedoc.h"

#include <classes.h>
#include "dbh.h"
Q_DECLARE_LOGGING_CATEGORY(ACDC)

namespace Ui {
class ActiveDoc;
}

class ActiveDoc : public QWidget, Ui::ActiveDoc
{
    Q_OBJECT
    
public:
    explicit ActiveDoc(QWidget *parent = 0);
    void _ActiveDoc(int idDoc,const QString &nameWorker,int idWorker,const QString &titleDoc, int totalPages, int pagesDone,QDate depositeDay,QDate deliveryDay);
    ~ActiveDoc();

public slots:
    void cancelAssociate();
    void saveToArchive();

private:
    DBH *DB;
    int idWorker;
    int idDoc;    
};

#endif // ACTIVEDOC_H
