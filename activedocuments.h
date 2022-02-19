#ifndef ACTIVEDOCUMENTS_H
#define ACTIVEDOCUMENTS_H

#include <QtWidgets/QWidget>
#include <QDate>
#include <QDebug>
#include <QSqlDatabase>
#include <QSqlQuery>

namespace Ui
{
    class ActiveDoc;
}

class ActiveDocuments : public QWidget
{
    Q_OBJECT

public:
    explicit ActiveDocuments(QWidget *parent = 0);
    void _ActiveDoc(int idDoc, const QString &nameWorker, int idWorker, const QString &titleDoc, int totalPages, int pagesDone, QDate depositeDay, QDate deliveryDay);
    ~ActiveDocuments();

public slots:
    void cancelAssociate();
    void saveToArchive();

private:
    Ui::ActiveDoc *ui;
    int idWorker;
    int idDocument;
};

#endif // ACTIVEDOCUMENTS_H
