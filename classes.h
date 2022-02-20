#ifndef CLASSES_H
#define CLASSES_H

#include <QtCore>
#include "toast.h"
#include "dialog.h"
#include <QGraphicsDropShadowEffect>
#include <QEvent>
#include <QKeyEvent>
#include <QCryptographicHash>
#include <QDir>
#include <QFile>
#include <QMessageBox>
#include "mdialog.h"

//! [ LOGGER ]
#include <QDebug>
#include <QtMessageHandler>
#include <QMessageLogger>
#include <QLoggingCategory>
#include <QDebugStateSaver>

//! [DBH QtSql ]
#include <QSqlQueryModel>
#include <QSqlQuery>
#include <QSqlTableModel>
#include <QSqlRelationalTableModel>
#include <QSortFilterProxyModel>
#include <QCryptographicHash>
#include <QDateTime>
#include <QSqlError>

//! [Qt Json ]
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonParseError>

//Q_DECLARE_LOGGING_CATEGORY(CLS)

//Q_LOGGING_CATEGORY(CLS,"CLASSES")

class Style
{
public:

    explicit Style();
    static QString loadStyle(const QString& file)
    {
        QFile MDFile(":"+file);
        if (MDFile.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            QTextStream in(&MDFile);
            return in.readAll().simplified();
        }
        return "";
    }

    static QGraphicsDropShadowEffect *shadowbutton()
    {
        QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect();
        shadow->setBlurRadius(8);
        shadow->setOffset(1);
        shadow->setColor(QColor(63, 63, 63, 180));
        return shadow;
    }

    static QGraphicsDropShadowEffect *shadow()
    {
        QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect();
        shadow->setBlurRadius(8);
        shadow->setOffset(3);
        shadow->setColor(QColor(63, 63, 63, 180));
        return shadow;
    }
};

class Worker
{

public:
    Worker()
    {
        clear();
    }

    void clear()
    {
        id = -1;
        fullName.clear();
        phoneNumber.clear();
        email.clear();
        remarke.clear();
        isDisponible = true;
        currnetDocID = -1;
        lastDocID = -1;
        nextDocID = -1;
        listDocsHundled.clear();
        date_created.clear();
        date_modified.clear();
    }

    int getID(){ return id; }
    QString getFullName(){ return fullName; }
    QString getPhoneNumber(){ return phoneNumber; }
    QString getEmail(){ return email; }
    QString getRemarke(){ return remarke; }
    bool getIsDisponible(){ return isDisponible; }
    int     getCurrnetDocID(){ return currnetDocID; }
    int     getLastDocID(){ return lastDocID; }
    int     getNextDocID(){ return nextDocID; }
    QList<int> getListDocsHundled(){ return listDocsHundled; }
    QString    getDateCreated(){ return date_created; }
    QString    getDateModified(){ return date_modified; }

    void setID(int value){ id = value; }
    void setFullName(const QString &text){  fullName = text; }
    void setPhoneNumber(const QString &text){ phoneNumber  = text; }
    void setEmail(const QString &text){  email = text; }
    void setRemarke(const QString &text){ remarke  = text; }
    void setIsDisponible(bool value){ isDisponible  = value; }
    void setCurrnetDocID(int value){ currnetDocID = value; }
    void setLastDocID(int value){ lastDocID = value; }
    void setNextDocID(int value){ nextDocID = value; }
    void setListDocsHundled(QList<int> list){ listDocsHundled = list; }
    void setDateCreated(const QString &text) { date_created  = text; }
    void setDateModified(const QString &text){ date_modified = text; }

private:
    int id;
    QString fullName;
    QString phoneNumber;
    QString email;
    QString remarke;
    bool isDisponible;
    int     currnetDocID;
    int     lastDocID;
    int     nextDocID;
    QList<int> listDocsHundled;
    QString    date_created;
    QString    date_modified;
};

class Document
{
private:
    int     id;
    int     writerID;
    int     ownerID;
    QString titleDoc;
    uint    totalPages;
    QString languageDoc;
    QString fontFamily;
    uint    fontSize;
    QString typeDoc;

    uint pagesDone;
    uint pagesWord;
    uint pagesHand;

    QString printingColor;
    bool    isPrinted;
    QString depositeDay;
    QString deliveryDay;
    bool    isDeliveredByMail;
    QString    dateFinished;
    QString    date_created;
    QString    date_modified;

public:
    Document()
    {
        clear();
    }

    void clear()
    {
        id = -1;
        writerID = -1;
        ownerID  = -1;
        titleDoc.clear();
        totalPages = 0 ;
        languageDoc.clear();
        fontFamily.clear();
        fontSize = 0;
        typeDoc.clear();
        pagesDone = 0;
        pagesHand = 0;
        pagesWord = 0;
        printingColor.clear();
        isPrinted = false;
        depositeDay.clear();
        deliveryDay.clear();
        isDeliveredByMail = false;
        dateFinished.clear();
        date_created.clear();
        date_modified.clear();
    }

    int     getID(){ return id;}
    int     getWriterID(){ return writerID; }
    int     getOwnerID(){  return ownerID;  }
    QString getTitleDoc(){ return titleDoc;}
    uint    getTotalPages(){  return totalPages;}
    QString getLanguageDoc(){ return languageDoc;}
    QString getFontFamily(){  return fontFamily;}
    uint    getFontSize(){ return fontSize;}
    QString getTypeDoc(){  return typeDoc;}
    uint    getPagesDone(){ return pagesDone; }
    uint    getPagesWord(){ return pagesWord; }
    uint    getPagesHand(){ return pagesHand; }
    QString getPrintingColor(){ return printingColor;}
    bool    getIsPrinted(){   return isPrinted; }
    QString getDepositeDay(){ return depositeDay;}
    QString getDeliveryDay(){ return deliveryDay;}
    bool    getIesDeliveredByMail(){ return isDeliveredByMail; }
    QString getDateFinished(){ return dateFinished; }
    QString getDateCreated(){  return date_created; }
    QString getDateModified(){ return date_modified;}

    void setID(int value){ id = value; }
    void setWriterID(int value){ writerID = value; }
    void setOwnerID(int value){  ownerID = value;  }
    void setTitleDoc(const QString &text){ titleDoc = text; }
    void setTotalPages(uint value){ totalPages = value; }
    void setLanguageDoc(const QString &text){ languageDoc = text; }
    void setFontFamily(const QString &text){  fontFamily = text; }
    void setFontSize(uint value){ fontSize = value; }
    void setTypeDoc(const QString &text){ typeDoc = text; }
    void setPagesDone(uint value){ pagesDone = value; }
    void setPagesWord(uint value){ pagesWord = value; }
    void setPagesHand(uint value){ pagesHand = value; }
    void setPrintingColor(const QString &text){ printingColor = text; }
    void setIsPrinted(bool value){ isPrinted = value; }
    void setDepositeDay(const QString &text){  depositeDay  = text; }
    void setDeliveryDay(const QString &text){  deliveryDay  = text; }
    void setIsDeliveredByMail(bool value){ isDeliveredByMail = value; }
    void setDateFinished(const QString &text){ dateFinished  = text; }
    void setDateCreated(const QString &text){ date_created   = text; }
    void setDateModified(const QString &text){ date_modified = text; }
};

class Client
{

private:
    int id;
    QString fullname;
    QString remarke;
    QString phoneNumber;
    QString email;

    Document *document;
    QList<Document*> listDocuments;

    QString payement_state;
    uint price;
    uint pricePaid;
    QString date_created;
    QString date_modified;

public:
    Client()
    {
        document = new Document();
        clear();   
    }

    void clear()
    {
        id = -1;
        fullname.clear();
        remarke.clear();
        phoneNumber.clear();
        email.clear();

        document = NULL;
        listDocuments.clear();

        payement_state.clear();
        price         = 0;
        pricePaid     = 0;
        date_created.clear();
        date_modified.clear();
    }

    int getID(){ return id; }
    QString getFullname(){ return fullname; }
    QString getRemarke(){ return remarke; }
    QString getPhoneNumber(){ return phoneNumber; }
    QString getEmail(){ return email; }

    Document* getDocument(){ return document; }
    QList<Document*> getListDocuments(){ return listDocuments; }

    QString getPayement_state(){ return payement_state; }
    uint getPrice(){ return price; }
    uint getPricePaid(){ return pricePaid; }
    QString getDateCreated(){ return date_created; }
    QString getDateModified(){ return date_modified; }

    void setID(int value){ id = value;  }
    void setFullname(const QString &text ){ fullname = text; }
    void setRemarke(const QString &text ){ remarke  = text; }
    void setPhoneNumber(const QString &text ){ phoneNumber = text; }
    void setEmail(const QString &text ){ email = text; }

    void setDocument(Document *doc){ document = doc; }
    void setListDocuments(QList<Document*> list ){ listDocuments  = list; }

    void setPayement_state(const QString &text){ payement_state  = text; }
    void setPrice(uint value){ price = value; }
    void setPricePaid(uint value){ pricePaid = value; }
    void setDateCreated(const QString &text){ date_created   = text; }
    void setDateModified(const QString &text){ date_modified = text; }
};

class QJson
{

public:
    explicit QJson(){ }

    static QJsonDocument *loadFile(const QString& name)
    {
        QFile mFile(":"+name);

        if (!mFile.open(QIODevice::ReadOnly)) {
            qDebug()<<"Couldn't open file : "<<name;
            return NULL;
        }

        QByteArray dataJson = mFile.readAll();

        QJsonParseError error;
        QJsonDocument *loadDoc = new QJsonDocument(QJsonDocument::fromJson(dataJson,&error));

        qInfo()<<"file : "<<name<<" opened.";

        mFile.close();
        return loadDoc;
    }

    static bool saveFile(const QString& name,QJsonDocument& jsonDoc)
    {
        QFile mFile(":"+name);

        if (!mFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
            qCritical()<<"Couldn't open file.";
            return false;
        }

        //QJsonDocument saveDoc(gameObject);
        mFile.write(jsonDoc.toJson());
        mFile.close();

        return true;
    }
};

#endif // CLASSES_H
