#include "dbh.h"
#include "simplecrypt.h"
#include <QDateTime>
#include <QtWidgets/QMessageBox>

Q_LOGGING_CATEGORY(DDB,"DB.MAIN")

DBH::DBH()
{
    connectionName = ""; // this helps mtransacton & mCommit.

    QSqlDatabase::addDatabase("QMYSQL");
    QSqlDatabase::database().setDatabaseName("BYASS");

//    QSqlDatabase::addDatabase("QSQLITE");
//    QSqlDatabase::database().setDatabaseName(QDir::homePath()+"/AppData/Roaming/bits/"+"BYASS.db");

    QSqlDatabase::database().setHostName("127.0.0.1");
    QSqlDatabase::database().setPort(3306);
    QSqlDatabase::database().setUserName("root");
    QSqlDatabase::database().setPassword("root");
    QSqlDatabase::database().open();
    qCDebug(DDB)<<"Using Default Connection To Open DB.";

    query = new QSqlQuery(QSqlDatabase::database());
}

DBH::DBH(const QString &name)
    : connectionName(name)
{
    QSqlDatabase::addDatabase("QMYSQL",name);
    QSqlDatabase::database(name).setDatabaseName("BYASS");

//    QSqlDatabase::addDatabase("QSQLITE",name);
//    QSqlDatabase::database(name).setDatabaseName(QDir::homePath()+"/AppData/Roaming/bits/"+"BYASS.db");

    QSqlDatabase::database(name).setHostName("127.0.0.1");
    QSqlDatabase::database(name).setPort(3306);
    QSqlDatabase::database(name).setUserName("root");
    QSqlDatabase::database(name).setPassword("root");
    QSqlDatabase::database(name).open();

    qCDebug(DDB)<<"List Of Active DB Connection"<<QSqlDatabase::connectionNames();

    query = new QSqlQuery(QSqlDatabase::database(name));
}

QSqlDatabase DBH::getCnx()
{
    if(connectionName.isEmpty())
        return QSqlDatabase::database();
    else
        return QSqlDatabase::database(connectionName);
}

void DBH::mTransaction()
{
    if(connectionName.isEmpty())
    {
        if(!QSqlDatabase::database().isOpen())
            QSqlDatabase::database().open();
        QSqlDatabase::database().transaction();
        query = new QSqlQuery(QSqlDatabase::database());
    }else
    {
        if(!QSqlDatabase::database(connectionName).isOpen())
            QSqlDatabase::database(connectionName).open();
        QSqlDatabase::database(connectionName).transaction();
        query = new QSqlQuery(QSqlDatabase::database(connectionName));
    }
}

void DBH::mCommit()
{
    if(connectionName.isEmpty())
        QSqlDatabase::database().commit();
    else
        QSqlDatabase::database(connectionName).commit();
}

void DBH::mRemoveDatabase(const QString& connectionName)
{
    QSqlDatabase::removeDatabase(connectionName);
}

void DBH::createEmptyDB()
{
    const static QString T_Lang = "(id INTEGER PRIMARY KEY AUTO_INCREMENT,"
                           "languageName VARCHAR(20),"
                           "abbrev VARCHAR(4))";

    const static QString T_Workers = "(id INTEGER PRIMARY KEY AUTO_INCREMENT,"
                           "fullName VARCHAR(50),"
                           "phoneNumber VARCHAR(10),"
                           "email varchar(100),"
                           "remarke VARCHAR(250),"
                           "isDisponible TINYINT(1),"
                           "currentDocID INTEGER,"
                           "date_created DATETIME,"
                           "date_modified DATETIME)";

    const static QString T_Documents = "(id INTEGER PRIMARY KEY AUTO_INCREMENT,"
                           "idClient INTEGER,"
                           "idWorker INTEGER,"
                           "titleDoc VARCHAR(250),"
                           "typeDoc VARCHAR(15),"
                           "fontFamily VARCHAR(50),"
                           "fontSize INTEGER,"
                           "printingColor VARCHAR(10),"
                           "isPrinted TINYINT(1),"
                           "languageDoc VARCHAR(30),"
                           "totalPages INTEGER,"
                           "pagesDone INTEGER,"
                           "depositeDay DATE,"
                           "deliveryDay DATE,"
                           "dateStarted DATE,"
                           "dateFinished DATE,"
                           "pagesHand INTEGER,"
                           "pagesWord INTEGER,"
                           "date_created DATETIME,"
                           "date_modified DATETIME)";

    const static QString T_Admin = "(id INTEGER PRIMARY KEY AUTO_INCREMENT,"
                           "app_name VARCHAR(50),"
                           "username VARCHAR(30),"
                           "password VARCHAR(40),"
                           "phoneNumber VARCHAR(14),"
                           "localAdress VARCHAR(150),"
                           "defaultPassword VARCHAR(40),"
                           "comment VARCHAR(250),"
                           "lastLogin TIMESTAMP,"
                           "timeLogs INTEGER,"
                           "date_created DATETIME,"
                           "date_modified DATETIME)";

    const static QString T_Clients = "(id INTEGER PRIMARY KEY AUTO_INCREMENT,"
                           "fullname VARCHAR(100),"
                           "phoneNumber VARCHAR(14),"
                           "firstEmail varchar(100),"
                           "isDeliveredByMail TINYINT(1),"
                           "remarke VARCHAR(600),"
                           "payement_state VARCHAR(50),"
                           "price INTEGER,"
                           "pricePaid INTEGER,"
                           "date_created DATETIME,"
                           "date_modified DATETIME)";

    // create the list tables of the schema.
    if(createTable("workers",T_Workers)) qCDebug(DDB)<<"Table workers OK.";
    if(!this->tables().contains("admin"))
    {
        createTable("admin",T_Admin);
        SimpleCrypt crypto(Q_UINT64_C(0x16af28db99bbca1f));
        QString name= crypto.encryptToString(QString("admin"));

        qCDebug(DDB)<<"Table Admin Created.";
        query->clear();
        query->prepare("INSERT INTO `admin` (app_name,username,password,"
                      "timeLogs,phoneNumber,date_created,date_modified)"
                      "VALUES(:app_name,:username,:password,"
                      ":timeLogs,:phoneNumber,:date_created,:date_modified)");

        query->bindValue(":app_name","yassin");
        query->bindValue(":username",name);
        query->bindValue(":password",name);
        query->bindValue(":phoneNumber", "----------");
        query->bindValue(":timeLogs", 0);
        query->bindValue(":date_created",QDateTime::currentDateTime().toString(Qt::ISODate));
        query->bindValue(":date_modified",QDateTime::currentDateTime().toString(Qt::ISODate));
        query->exec();
    }
    if(createTable("documents",T_Documents)) qCDebug(DDB)<<"Table Documents OK.";
    if(createTable("clients",T_Clients))     qCDebug(DDB)<<"Table Clients OK.";
}

int  DBH::createTable(const QString &nameTable,const QString &tableQueryStructure)
{
    int id = -1;
    mTransaction();

    query->clear();
    query->exec("CREATE TABLE IF NOT EXISTS `"+nameTable+"` "+tableQueryStructure);

    anyError(query->lastError());

    mCommit();
    return id;
}

void DBH::anyError(QSqlError e)
{
    switch (e.type())
    {
        case QSqlError::NoError:
            //id= 1;
            qCDebug(DDB)<<"All Good.";
            break;
        case QSqlError::ConnectionError:
            qCDebug(DDB)<<"Error Connection To DB.";
            break;
        case QSqlError::StatementError:
            qCDebug(DDB)<<"Error Statement Sql.";
            break;
        case QSqlError::TransactionError:
            qCDebug(DDB)<<"Error Transaction.";
        case QSqlError::UnknownError:
            qCDebug(DDB)<<"Unknown Error.";
            break;
        default:
            break;
    }
}

bool DBH::attemptLogin(const QString &username, const QString &password)
{
    QString tmp_name = "";
    QString tmp_pass = "";

    mTransaction();
    SimpleCrypt crypto(Q_UINT64_C(0x16af28db99bbca1f));

    query->clear();
    query->prepare("SELECT `username`,`password` FROM `admin`");
    query->exec();

    if (query->next())
    {
        tmp_name  =  crypto.decryptToString(query->value("username").toString());
        tmp_pass  =  crypto.decryptToString(query->value("password").toString());
    }

    qCDebug(DDB)<<tmp_name<<" :: "<<tmp_pass;
    mCommit();

    return (tmp_name == username && tmp_pass == password);
}

int DBH::updateClient(Client* data)
{
    int val = -1;
    mTransaction();

    query->clear();
    query->prepare("UPDATE `clients` SET "
                   "`fullname`=:fullname,`phoneNumber`=:phoneNumber,`firstEmail`=:firstEmail,"
                   "`payement_state`=:payement_state,`price`=:price,`pricePaid`=:pricePaid WHERE "
                   "`id`=:id");

    query->bindValue(":id",data->getID());
    query->bindValue(":fullname",data->getFullname());
    query->bindValue(":phoneNumber", data->getPhoneNumber());
    query->bindValue(":firstEmail", data->getEmail());
    query->bindValue(":payement_state", data->getPayement_state());
    query->bindValue(":price", data->getPrice());
    query->bindValue(":pricePaid", data->getPricePaid());

    query->exec();

    if(query->lastError().type() == QSqlError::NoError)
        val = 1;
    else
        qCDebug(DDB)<<query->lastError().type();

    mCommit();
    return val;
}

int DBH::addClient(Client* data)
{
    int idClient = -1;
    mTransaction();

    query->clear();
    query->prepare("INSERT INTO `clients` ("
                   "`fullname`,`phoneNumber`,`firstEmail`,"
                   "`remarke`,`payement_state`,`price`,"
                   "`pricePaid`,`date_created`,`date_modified`)"
                   " VALUES ("
                   ":fullname,:phoneNumber,:firstEmail,"
                   ":remarke,:payement_state,:price,"
                   ":pricePaid,:date_created,:date_modified)");

    query->bindValue(":fullname",data->getFullname());
    query->bindValue(":phoneNumber",data->getPhoneNumber());
    query->bindValue(":firstEmail",data->getEmail());
    query->bindValue(":remarke", data->getRemarke());
    query->bindValue(":payement_state",data->getPayement_state());
    query->bindValue(":price",data->getPrice());
    query->bindValue(":pricePaid",data->getPricePaid());
    query->bindValue(":date_created",data->getDateCreated());
    query->bindValue(":date_modified",data->getDateModified());

    query->exec();
    idClient = query->lastInsertId().toInt();

    // anyError(query->lastError());

    mCommit();

    return idClient;
}

int DBH::deleteClientByID(int idClient)
{
    int rowDeleted = -1;
    mTransaction();

    int idDoc = 0;
    int idWrkr= 0;

    query->clear();
    query->prepare("SELECT `id`,`idWorker` FROM `documents` WHERE `idClient`=:idClient ");
    query->bindValue(":idClient",idClient);
    query->exec();

    while(query->next())
    {
        idDoc  =  query->value("id").toInt();
        idWrkr =  query->value("idWorker").toInt();
    }

    query->clear();
    query->prepare("UPDATE `workers` SET `currentDocID`= 0 WHERE `id`= :id ");
    query->bindValue(":id",idWrkr);
    query->exec();

    query->clear();
    query->prepare("DELETE FROM `documents` WHERE `id`= :id ");
    query->bindValue(":id",idDoc);
    query->exec();

    query->clear();
    query->prepare("DELETE FROM `clients` WHERE `id`= :id ");
    query->bindValue(":id",idClient);
    query->exec();

    if(query->lastError().type() == QSqlError::NoError)
        rowDeleted = 1;
    else
        qCDebug(DDB)<<query->lastError().type();

    mCommit();

    return rowDeleted;
}

int DBH::addDocument(int idClient,Document* doc)
{
    int idDocument = -1;

    mTransaction();

    query->clear();
    query->prepare("INSERT INTO `documents` ("
                   "`idClient`,`idWorker`,`titleDoc`, `typeDoc`,"
                   "`fontFamily`,`fontSize`,`printingColor`,"
                   "`isPrinted`,`languageDoc`,`totalPages`,"
                   "`pagesDone`,`depositeDay`,`deliveryDay`,"
                   "`dateStarted`,`dateFinished`,`pagesHand`,"
                   "`pagesWord`,`date_created`,`date_modified`)"
                   "VALUES("
                   ":idClient ,NULL,:titleDoc,:typeDoc,"
                   ":fontFamily,:fontSize,:printingColor,"
                   ":isPrinted,:languageDoc,:totalPages,"
                   ":pagesDone,:depositeDay,:deliveryDay,"
                   "NULL ,NULL  ,NULL,"
                   "NULL ,:date_created, :date_modified)"
                   );

    query->bindValue(":idClient",  idClient);
//    query->bindValue(":idWorker",  "NULL");
    query->bindValue(":titleDoc",  doc->getTitleDoc());
    query->bindValue(":typeDoc",   doc->getTypeDoc());
    query->bindValue(":fontFamily",doc->getFontFamily());
    query->bindValue(":fontSize",  doc->getFontSize());
    query->bindValue(":printingColor", doc->getPrintingColor());
    query->bindValue(":isPrinted", doc->getIsPrinted());
    query->bindValue(":languageDoc", doc->getLanguageDoc());
    query->bindValue(":totalPages",  doc->getTotalPages());
    query->bindValue(":pagesDone",0);
    query->bindValue(":depositeDay", doc->getDepositeDay());
    query->bindValue(":deliveryDay", doc->getDeliveryDay());
//    query->bindValue(":dateStarted",  "NULL");
//    query->bindValue(":dateFinished", "NULL");
//    query->bindValue(":pagesHand","NULL");
//    query->bindValue(":pagesWord","NULL");
    query->bindValue(":date_created",  doc->getDateCreated());
    query->bindValue(":date_modified", doc->getDateModified());
    query->exec();

    anyError(query->lastError());
    qCDebug(DDB)<<query->lastQuery();

    idDocument = query->lastInsertId().toInt();

    mCommit();
    return idDocument;
}

int DBH::updateDocument(Document* doc)
{
    int idDoc = -1;
    mTransaction();

    query->clear();
    query->prepare("UPDATE `documents` SET `pagesDone`= :pagesDone, `pagesHand`= :pagesHand, "
                   "`pagesWord`= :pagesWord, `isPrinted`= :isPrinted, `deliveryDay`= :deliveryDay"
                   " WHERE `id`=:id");

    query->bindValue(":id",         doc->getID());
    query->bindValue(":pagesDone",  doc->getPagesDone());
    query->bindValue(":pagesHand",  doc->getPagesHand());
    query->bindValue(":pagesWord",  doc->getPagesWord());
    query->bindValue(":isPrinted",  doc->getIsPrinted());
    query->bindValue(":deliveryDay",doc->getDeliveryDay());

    query->exec();

    if(query->lastError().type() == QSqlError::NoError)
        idDoc = 1;
    else
        qCDebug(DDB)<<query->lastError().type();

    mCommit();
    return idDoc;
}

QList<Document*> DBH::getAvailableDocsList()
{
    QList<Document*> list;
    list.clear();
    Document* doc = new Document();

    mTransaction();

    query->clear();
    query->prepare("SELECT `id`,`idClient`,`titleDoc` FROM `documents` "
                   "WHERE `dateStarted` IS NULL ORDER BY `titleDoc` ASC ");
    query->exec();

    while (query->next())
    {
        doc->setID(query->value("id").toInt());
        doc->setTitleDoc(query->value("titleDoc").toString());
        list<<doc;
    }

    mCommit();
    return list;
}

QList<Worker*> DBH::getWorkersList()
{
    QList<Worker*> list;
    list.clear();
    Worker* wk;

    mTransaction();

    query->clear();
    query->prepare("SELECT `id`,`fullName` FROM `workers`"
                   "WHERE `isDisponible`= :isDisponible ");

    query->bindValue(":isDisponible",true);
    query->exec();

    while (query->next())
    {
        wk = new Worker();
        wk->setID(query->value("id").toInt());
        wk->setFullName(query->value("fullName").toString());
        list<<wk;
    }

    mCommit();
    return list;
}

bool DBH::assignDoc2Worker(const QString &workerName,const QString &docTitle)
{
    int idWorker = -1;
    int idDoc    = -1;
    mTransaction();

    //! Get Both Indexes Document & Worker.
    query->clear();
    query->prepare("SELECT `id` FROM `workers` WHERE `fullName`=:fullName ");
    query->bindValue(":fullName",workerName);
    query->exec();
    if(query->next())
        idWorker = query->value("id").toInt();

    query->clear();
    query->prepare("SELECT `id` FROM `documents` WHERE `titleDoc`=:titleDoc ");
    query->bindValue(":titleDoc",docTitle);
    query->exec();
    if(query->next())
        idDoc = query->value("id").toInt();

    //! Update Necessery Data in table "workers/documents".
    query->clear();
    query->prepare("UPDATE `documents` SET `pagesDone`=:pagesDone,`dateStarted`=:dateStarted,"
                   "idWorker =:idWorker WHERE id=:id ");

    query->bindValue(":pagesDone",0);
    query->bindValue(":dateStarted",QDate::currentDate().toString("yyyy-MM-dd"));
    query->bindValue(":idWorker",idWorker);
    query->bindValue(":id",idDoc);
    query->exec();

    query->clear();
    query->prepare("UPDATE `workers` SET `currentDocID`=:currentDocID WHERE id=:id ");
    query->bindValue(":currentDocID",idDoc);
    query->bindValue(":id",idWorker);
    query->exec();

    mCommit();
    return (idWorker != -1);
}

QList<Document*> DBH::getActiveDocsList()
{
    QList<Document*> list;
    list.clear();
    Document *doc = new Document();

    mTransaction();

    query->clear();
    query->prepare("SELECT `id`,`idWorker`,`titleDoc`,"
                   "`totalPages`,`pagesDone`,`depositeDay`,"
                   "`deliveryDay` FROM `documents` WHERE "
                   "`dateStarted` IS NOT NULL "); // AND `dateFinished` IS NOT NULL

//    query->bindValue(":dateStarted",-1);
//    query->bindValue(":dateFinished",-1);
    query->exec();

    while(query->next())
    {
        doc->clear();
        doc->setID(query->value("id").toInt());
        doc->setWriterID(query->value("idWorker").toInt());
        doc->setTitleDoc(query->value("titleDoc").toString());
        doc->setTotalPages(query->value("totalPages").toInt());
        doc->setPagesDone(query->value("pagesDone").toInt());
        doc->setDepositeDay(query->value("depositeDay").toString());
        doc->setDeliveryDay(query->value("deliveryDay").toString());
        // QDate::fromString(query->value("deliveryDay").toString(),"yyyy-MM-dd")
        list<<doc;
    }
    mCommit();
    return list;
}

QString DBH::getWorkerNameByID(int id)
{
    QString name;
    name.clear();

    mTransaction();

    query->clear();
    query->prepare("SELECT `fullName` FROM `workers` WHERE `id`= :id ");
    query->bindValue(":id",id);
    query->exec();

    if (query->next())
         name = query->value("fullName").toString();

    mCommit();

    return name;
}

QList<Document*> DBH::getArchivedDocsList()
{
    QList<Document*> list;
    list.clear();
    Document* doc = new Document();

    mTransaction();

    query->clear();
    query->prepare("SELECT `id`,`idClient`,`titleDoc`,`pagesDone` "
                   "FROM `documents` WHERE `dateFinished` <> :dateFinished");
    query->bindValue(":dateFinished",-1);
    query->exec();

    while(query->next())
    {
        doc->clear();
        doc->setID(query->value("id").toInt());
        doc->setOwnerID(query->value("idClient").toInt());
        doc->setTitleDoc(query->value("titleDoc").toString());
        doc->setPagesDone(query->value("pagesDone").toInt());
        list<<doc;
    }

    mCommit();
    return list;
}

QString DBH::getClientNameByID(int id)
{
    mTransaction();

    QString clientName;
    clientName.clear();

    query->clear();
    query->prepare("SELECT `fullname` FROM `clients` WHERE `id`=:id");
    query->bindValue(":fullname",id);
    query->exec();

    if(query->next())
        clientName = query->value("fullname").toString();
    mCommit();

    return clientName;
}

int DBH::updateWorker(Worker* data)
{
    int rowsUpdated = -1;
    mTransaction();

    query->clear();
    query->prepare("UPDATE `workers` SET `fullName`=:fullName,"
                   "`phoneNumber`=:phoneNumber,`email`=:email,"
                   "`remarke`=:remarke,`isDisponible`=:isDisponible WHERE "
                   "`id`=:id");

    query->bindValue(":id",          data->getID());
    query->bindValue(":fullName",    data->getFullName());
    query->bindValue(":phoneNumber", data->getPhoneNumber());
    query->bindValue(":email",       data->getEmail());
    query->bindValue(":remarke",     data->getRemarke());
    query->bindValue(":isDisponible",data->getIsDisponible());

    query->exec();
    rowsUpdated = query->numRowsAffected();

    mCommit();
    return rowsUpdated;
}

int DBH::addWorker(Worker* data)
{
    int idWorker = -1;

    mTransaction();

    query->clear();
    query->prepare("INSERT INTO `workers` ("
                   "`id`,`fullName`,`phoneNumber`,`email`,"
                   "`remarke`, `isDisponible`,`currentDocID`,"
                   "`date_created`,`date_modified`) VALUES ("
                   "NULL, :fullName, :phoneNumber, :email,:remarke,"
                   "NULL, NULL, :date_created, :date_modified);");

    query->bindValue(":fullName",   data->getFullName());
    query->bindValue(":phoneNumber",data->getPhoneNumber());
    query->bindValue(":email",      data->getEmail());
    query->bindValue(":remarke",    data->getRemarke());
    query->bindValue(":isDisponible", data->getIsDisponible());
    query->bindValue(":date_created", data->getDateCreated());
    query->bindValue(":date_modified",data->getDateModified());
    query->exec();

    anyError(query->lastError());
    idWorker = query->lastInsertId().toInt();

    mCommit();
    return idWorker;
}

int DBH::deleteWorkerbyID(int id)
{
    int rowDeleted = -1;
    mTransaction();

    int currentDocID = 0;

    query->clear();
    query->prepare("SELECT `currentDocID` FROM `workers` WHERE `id`= :id");
    query->bindValue(":id",id);
    query->exec();
    if (query->next())
            currentDocID  =  query->value("currentDocID").toInt();

    query->clear();
    query->prepare("UPDATE `documents` SET `idWorker`=:idWorker WHERE `id`=:id "
                   "AND totalPages<>pagesDone");
    query->bindValue(":idWorker",-1);
    query->bindValue(":id",currentDocID);
    query->exec();

    query->clear();
    query->prepare("SELECT `id` FROM `documents` WHERE `idWorker`= :idWorker");
    query->bindValue(":idWorker",id);
    query->exec();

    if(query->size()>0)
    {
        query->clear();
        query->prepare("UPDATE `workers` SET `isDisponible`='NO' WHERE `id`=:id");
        query->bindValue(":id",id);
        query->exec();
    }else
    {
        query->clear();
        query->prepare("DELETE FROM `workers` WHERE `id`= :id");
        query->bindValue(":id",id);
        query->exec();
        rowDeleted = query->numRowsAffected();
    }

    mCommit();
    return rowDeleted;
}

int DBH::updateAdminName(const QString &name)
{
    int rowUpdated = -1;
    mTransaction();

    SimpleCrypt crypto(Q_UINT64_C(0x16af28db99bbca1f));

    query->clear();
    query->prepare("UPDATE `admin` SET `username`= :username WHERE `app_name`='yassin' ");
    query->bindValue(":username",crypto.encryptToString(name));
    query->exec();
    rowUpdated = query->numRowsAffected();

    mCommit();
    return rowUpdated;
}

int DBH::updateAdminPassword(const QString &password)
{
    int rowUpdated = -1;
    mTransaction();

    SimpleCrypt crypto(Q_UINT64_C(0x16af28db99bbca1f));

    query->clear();
    query->prepare("UPDATE `admin` SET `password`= :password WHERE `app_name`='yassin' ");
    query->bindValue(":password",crypto.encryptToString(password));
    query->exec();
    rowUpdated = query->numRowsAffected();

    mCommit();
    return rowUpdated;
}

int DBH::updateAdminAddress(const QString &address)
{
    int rowUpdated = -1;
    mTransaction();

    query->clear();
    query->prepare("UPDATE `admin` SET `localAdress`= :localAdress WHERE `app_name`='yassin' ");
    query->bindValue(":localAdress",address);
    query->exec();
    rowUpdated = query->numRowsAffected();

    mCommit();
    return rowUpdated;
}

int DBH::updateAdminPhone(const QString &phone)
{
    int rowUpdated = -1;
    mTransaction();

    query->clear();
    query->prepare("UPDATE `admin` SET `phoneNumber`= :phoneNumber WHERE `app_name`='yassin' ");
    query->bindValue(":phoneNumber",phone);
    query->exec();
    rowUpdated = query->numRowsAffected();

    mCommit();
    return rowUpdated;
}
