#ifndef ARCHIVEDDOCUMENTS_H
#define ARCHIVEDDOCUMENTS_H

#include <QtWidgets/QWidget>
#include <QDate>
#include <QDebug>
#include <QSqlDatabase>
#include <QSqlQuery>

namespace Ui
{
    class ArchivedDoc;
}

class ArchivedDocuments : public QWidget
{
    Q_OBJECT

public:
    explicit ArchivedDocuments(QWidget *parent = 0);
    ~ArchivedDocuments();

public slots:
    void showArchivedDocuments(int idDoc, const QString &nameClient, const QString &titleDoc, int pagesDone);
    void removeDocumentFromArchive();

private:
    Ui::ArchivedDoc *ui;
    int idDocument;
};

#endif // ARCHIVEDDOCUMENTS_H
