#ifndef ACTIVEDOC_H
#define ACTIVEDOC_H

#include <QtWidgets/QWidget>
#include <QDate>
#include <QDebug>
#include <QSqlDatabase>
#include <QSqlQuery>


namespace Ui {
class ActiveDoc;
}

class ActiveDoc : public QWidget
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
    Ui::ActiveDoc *ui;
    QSqlDatabase DBH;
    int idWorker;
    int idDoc;    
};

#endif // ACTIVEDOC_H
