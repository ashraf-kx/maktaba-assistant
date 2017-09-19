#ifndef ARCHIVEDDOC_H
#define ARCHIVEDDOC_H

#include <QtWidgets/QWidget>
#include "ui_archiveddoc.h"

#include <classes.h>
#include "dbh.h"
Q_DECLARE_LOGGING_CATEGORY(ARDC)

namespace Ui {
class ArchivedDoc;
}

class ArchivedDoc : public QWidget, Ui::ArchivedDoc
{
    Q_OBJECT
    
public:
    explicit ArchivedDoc(QWidget *parent = 0);
    ~ArchivedDoc();

public slots:
    void showArchivedDoc(int idDoc);
    void removeFromArchive();
private:
    DBH *DB;
    int idDoc;
};

#endif // ARCHIVEDDOC_H
