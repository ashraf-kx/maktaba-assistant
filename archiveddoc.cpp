#include "archiveddoc.h"
#include <QDir>

Q_LOGGING_CATEGORY(ARDC,"ARCHIVE")

ArchivedDoc::ArchivedDoc(QWidget *parent) :
    QWidget(parent)
{
    setupUi(this);

    DB = new DBH("_Archive_");

    this->setGraphicsEffect(Style::shadow());

    connect(Bt_Cancel,SIGNAL(pressed()),this,SLOT(removeFromArchive()));
    connect(Bt_Cancel,SIGNAL(pressed()),parent,SLOT(updateLists()));
    connect(Bt_Cancel,SIGNAL(pressed()),parent,SLOT(currentActiveStats()));
    connect(Bt_Cancel,SIGNAL(released()),this,SLOT(deleteLater()));
}

ArchivedDoc::~ArchivedDoc()
{

}

void ArchivedDoc::removeFromArchive()
{
    DB->removeFromArchive(idDoc);
}

void ArchivedDoc::showArchivedDoc(int idDoc/*, const QString &clientName, const QString &docTitle,
                                   int pagesDone*/)
{
    this->idDoc = idDoc;

    Client   *data = DB->getClientAndDocByID(idDoc);
    Document *doc  = data->getDocument();

    // Apply Calculations On The Data Colleted.
    Lb_nameClient->setText(data->getFullname());
    Lb_titleBook->setText(doc->getTitleDoc());

    if(doc->getPagesDone() < doc->getTotalPages() ){
        Lb_pages->setStyleSheet("QLabel{background-color:gray;}");
    }else{
        Lb_pages->setStyleSheet("QLabel{background-color:#55ff00;}");
    }

    if(doc->getIsPrinted())
        Lb_print->setStyleSheet("QLabel{background-color:#55ff00;}");
    else
        Lb_print->setStyleSheet("QLabel{background-color:gray;}");

    if(data->getPrice() == data->getPricePaid())
        Lb_dollar->setStyleSheet("QLabel{background-color:#55ff00;}");
    else
        Lb_dollar->setStyleSheet("QLabel{background-color:gray;}");
}
