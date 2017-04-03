#ifndef ARCHIVEDDOC_H
#define ARCHIVEDDOC_H

#include <QWidget>
#include <QDate>
#include <QDebug>
#include <QSqlDatabase>
#include <QSqlQuery>

namespace Ui {
class ArchivedDoc;
}

class ArchivedDoc : public QWidget
{
    Q_OBJECT
    
public:
    explicit ArchivedDoc(QWidget *parent = 0);
    ~ArchivedDoc();

public slots:
    void showArchivedDocs(int idDoc,const QString &nameClient,const QString &titleDoc,int pagesDone);
    void removeFromArchive();
private:
    Ui::ArchivedDoc *ui;
    QSqlDatabase DBH;
    int idDoc;
};

#endif // ARCHIVEDDOC_H
