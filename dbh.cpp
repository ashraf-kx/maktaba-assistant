#include "dbh.h"
#include "simplecrypt.h"
#include <QDateTime>
#include <QtWidgets/QMessageBox>

Q_LOGGING_CATEGORY(DDB, "DB.MAIN")

DBH::DBH()
{    
    if(!QSqlDatabase::contains(QSqlDatabase::defaultConnection))
    {
        QJsonObject jObj = QJson::loadFile("postgres_db")->object();

        qCInfo(DDB) << "Json: "
                    << jObj["driver"].toString()
                    << jObj["db_name"].toString()
                    << jObj["host"].toString()
                    << jObj["port"].toInt()
                    << jObj["username"].toString()
                    << jObj["password"].toString();

        qDebug()<<"Establish the default database connection.";
        QSqlDatabase connection = QSqlDatabase::addDatabase(jObj["driver"].toString());

        connection.setDatabaseName(jObj["db_name"].toString());
        connection.setHostName(jObj["host"].toString());
        connection.setPort(jObj["port"].toInt());
        connection.setUserName(jObj["username"].toString());
        connection.setPassword(jObj["password"].toString());
    }    

    if (QSqlDatabase::database().isOpen())
        qCInfo(DDB) << "[ Connection status: active ].";
    else
    {
        if(QSqlDatabase::database().open())
            qCInfo(DDB) << "Connection to database has been established.";
        else
            qCDebug(DDB) << "Couldn't open connection to database.";
    }

    query = new QSqlQuery(QSqlDatabase::database());
}

QSqlDatabase DBH::getConnection()
{
    return QSqlDatabase::database();
}

void DBH::createEmptyDB()
{
    QString workersTable, documentsTable, adminTable, clientsTable;

    qCInfo(DDB) << "Driver used: QPSQL.";
    workersTable = "(id SERIAL,"
                   "fullName varchar(50),"
                   "phoneNumber varchar(10),"
                   "email varchar(100),"
                   "remarke varchar(250),"
                   "isDisponible boolean,"
                   "currentDocID INTEGER,"
                   "date_created TIMESTAMP,"
                   "date_modified TIMESTAMP)";

    documentsTable = "(id SERIAL,"
                     "idClient INTEGER,"
                     "idWorker INTEGER,"
                     "titleDoc VARCHAR(250),"
                     "typeDoc VARCHAR(15),"
                     "fontFamily VARCHAR(50),"
                     "fontSize INTEGER,"
                     "printingColor VARCHAR(10),"
                     "isPrinted boolean,"
                     "languageDoc VARCHAR(30),"
                     "totalPages INTEGER,"
                     "pagesDone INTEGER,"
                     "depositeDay DATE,"
                     "deliveryDay DATE,"
                     "dateStarted DATE,"
                     "dateFinished DATE,"
                     "pagesHand INTEGER,"
                     "pagesWord INTEGER,"
                     "date_created TIMESTAMP,"
                     "date_modified TIMESTAMP)";

    adminTable = "(id SERIAL,"
                 "app_name VARCHAR(50),"
                 "username VARCHAR(30),"
                 "password VARCHAR(40),"
                 "phoneNumber VARCHAR(14),"
                 "localAdress VARCHAR(150),"
                 "defaultPassword VARCHAR(40),"
                 "comment VARCHAR(250),"
                 "lastLogin TIMESTAMP,"
                 "timeLogs INTEGER,"
                 "date_created TIMESTAMP,"
                 "date_modified TIMESTAMP)";

    clientsTable = "(id SERIAL,"
                   "fullname VARCHAR(100),"
                   "phoneNumber VARCHAR(14),"
                   "firstEmail varchar(100),"
                   "isDeliveredByMail boolean,"
                   "remarke VARCHAR(600),"
                   "payement_state VARCHAR(50),"
                   "price INTEGER,"
                   "pricePaid INTEGER,"
                   "date_created TIMESTAMP,"
                   "date_modified TIMESTAMP)";

    QSqlDatabase connection = QSqlDatabase::database();
    const QStringList tables = connection.tables();

    if(connection.isOpen() || connection.open())
    {

        connection.transaction();
        if (!tables.contains("workers"))
        {
            query->exec("CREATE TABLE workers " % workersTable);
            anyError(query->lastError());
            query->clear();

            qCDebug(DDB) << "Table workers created.";
        }
        else
            qCDebug(DDB) << "Table workers already exists.";

        if (!tables.contains("admin"))
        {
            query->exec("CREATE TABLE admin " % adminTable);

            anyError(query->lastError());
            qCDebug(DDB) << "Table admin created.";

            SimpleCrypt crypto(Q_UINT64_C(0x16af28db99bbca1f));
            QString name = crypto.encryptToString(QString("admin"));

            query->clear();
            query->prepare("INSERT INTO admin (app_name,username,password,"
                           "timeLogs,phoneNumber,date_created,date_modified)"
                           "VALUES(:app_name,:username,:password,"
                           ":timeLogs,:phoneNumber,:date_created,:date_modified)");

            query->bindValue(":app_name", "yassin");
            query->bindValue(":username", name);
            query->bindValue(":password", name);
            query->bindValue(":phoneNumber", "----------");
            query->bindValue(":timeLogs", 0);
            query->bindValue(":date_created", QDateTime::currentDateTime().toString(Qt::ISODate));
            query->bindValue(":date_modified", QDateTime::currentDateTime().toString(Qt::ISODate));
            query->exec();
            query->clear();
        }
        else
            qCDebug(DDB) << "Table admin already exists.";

        if (!tables.contains("workers"))
        {
            query->exec("CREATE TABLE documents " % documentsTable);
            anyError(query->lastError());
            query->clear();
            qCDebug(DDB) << "Table documents created.";
        }
        else
            qCDebug(DDB) << "Table documents already exists.";

        if (!tables.contains("workers"))
        {
            query->exec("CREATE TABLE clients " % clientsTable);
            anyError(query->lastError());
            query->clear();
            qCDebug(DDB) << "Table clients created.";
        }
        else
            qCDebug(DDB) << "Table clients already exists.";

        connection.commit();
    }
}

void DBH::anyError(QSqlError e)
{
    switch (e.type())
    {
    case QSqlError::NoError:
        qCInfo(DDB) << "OK.";
        break;
    case QSqlError::ConnectionError:
        qCDebug(DDB) << "Error Connection To DB.";
        break;
    case QSqlError::StatementError:
        qCDebug(DDB) << "Error Statement Sql.";
        break;
    case QSqlError::TransactionError:
        qCDebug(DDB) << "Error Transaction.";
        break;
    case QSqlError::UnknownError:
        qCDebug(DDB) << "Unknown Error.";
        break;
    default:
        break;
    }
}

bool DBH::attemptLogin(const QString &username, const QString &password)
{
    QString tmp_name = "";
    QString tmp_pass = "";

    SimpleCrypt crypto(Q_UINT64_C(0x16af28db99bbca1f));

    query->prepare("SELECT username,password FROM admin");
    query->exec();

    if (query->next())
    {
        tmp_name = crypto.decryptToString(query->value("username").toString());
        tmp_pass = crypto.decryptToString(query->value("password").toString());
    }

    qCInfo(DDB) << tmp_name << " :: " << tmp_pass;
    query->clear();
    return (tmp_name == username && tmp_pass == password);
}

int DBH::updateClient(Client *data)
{
    int val = -1;

    query->clear();
    query->prepare("UPDATE clients SET "
                   "fullname=:fullname,phoneNumber=:phoneNumber,firstEmail=:firstEmail,"
                   "payement_state=:payement_state,price=:price,pricePaid=:pricePaid WHERE "
                   "id=:id");

    query->bindValue(":id", data->getID());
    query->bindValue(":fullname", data->getFullname());
    query->bindValue(":phoneNumber", data->getPhoneNumber());
    query->bindValue(":firstEmail", data->getEmail());
    query->bindValue(":payement_state", data->getPayement_state());
    query->bindValue(":price", data->getPrice());
    query->bindValue(":pricePaid", data->getPricePaid());

    query->exec();

    if (query->lastError().type() == QSqlError::NoError)
        val = 1;
    else
        qCDebug(DDB) << query->lastError().type();

    return val;
}

int DBH::addClient(Client *data)
{
    int idClient = -1;

    query->prepare("INSERT INTO clients ("
                   "fullname,phoneNumber,firstEmail,"
                   "remarke,payement_state,price,"
                   "pricePaid,date_created,date_modified)"
                   " VALUES ("
                   ":fullname,:phoneNumber,:firstEmail,"
                   ":remarke,:payement_state,:price,"
                   ":pricePaid,:date_created,:date_modified)");

    query->bindValue(":fullname", data->getFullname());
    query->bindValue(":phoneNumber", data->getPhoneNumber());
    query->bindValue(":firstEmail", data->getEmail());
    query->bindValue(":remarke", data->getRemarke());
    query->bindValue(":payement_state", data->getPayement_state());
    query->bindValue(":price", data->getPrice());
    query->bindValue(":pricePaid", data->getPricePaid());
    query->bindValue(":date_created", data->getDateCreated());
    query->bindValue(":date_modified", data->getDateModified());

    query->exec();
    idClient = query->lastInsertId().toInt();

    // anyError(query->lastError());
    query->clear();

    return idClient;
}

int DBH::deleteClientByID(int idClient)
{
    int rowDeleted = -1;

    QSqlDatabase connection = QSqlDatabase::database();
    if(connection.isOpen() || connection.open())
    {

        connection.transaction();

        int idDoc = 0;
        int idWrkr = 0;

        query->clear();
        query->prepare("SELECT id,idWorker FROM documents WHERE idClient=:idClient ");
        query->bindValue(":idClient", idClient);
        query->exec();

        while (query->next())
        {
            idDoc = query->value("id").toInt();
            idWrkr = query->value("idWorker").toInt();
        }

        query->clear();
        query->prepare("UPDATE workers SET currentDocID= 0 WHERE id= :id ");
        query->bindValue(":id", idWrkr);
        query->exec();

        query->clear();
        query->prepare("DELETE FROM documents WHERE id= :id ");
        query->bindValue(":id", idDoc);
        query->exec();

        query->clear();
        query->prepare("DELETE FROM clients WHERE id= :id ");
        query->bindValue(":id", idClient);
        query->exec();

        if (query->lastError().type() == QSqlError::NoError)
            rowDeleted = 1;
        else
            qCDebug(DDB) << query->lastError().type();

        connection.commit();
    }

    return rowDeleted;
}

int DBH::addDocument(int idClient, Document *doc)
{
    int idDocument = -1;

    query->clear();
    query->prepare("INSERT INTO documents ("
                   "idClient,idWorker,titleDoc, typeDoc,"
                   "fontFamily,fontSize,printingColor,"
                   "isPrinted,languageDoc,totalPages,"
                   "pagesDone,depositeDay,deliveryDay,"
                   "dateStarted,dateFinished,pagesHand,"
                   "pagesWord,date_created,date_modified)"
                   "VALUES("
                   ":idClient ,NULL,:titleDoc,:typeDoc,"
                   ":fontFamily,:fontSize,:printingColor,"
                   ":isPrinted,:languageDoc,:totalPages,"
                   ":pagesDone,:depositeDay,:deliveryDay,"
                   "NULL ,NULL  ,NULL,"
                   "NULL ,:date_created, :date_modified)");

    query->bindValue(":idClient", idClient);
    //    query->bindValue(":idWorker",  "NULL");
    query->bindValue(":titleDoc", doc->getTitleDoc());
    query->bindValue(":typeDoc", doc->getTypeDoc());
    query->bindValue(":fontFamily", doc->getFontFamily());
    query->bindValue(":fontSize", doc->getFontSize());
    query->bindValue(":printingColor", doc->getPrintingColor());
    query->bindValue(":isPrinted", doc->getIsPrinted());
    query->bindValue(":languageDoc", doc->getLanguageDoc());
    query->bindValue(":totalPages", doc->getTotalPages());
    query->bindValue(":pagesDone", 0);
    query->bindValue(":depositeDay", doc->getDepositeDay());
    query->bindValue(":deliveryDay", doc->getDeliveryDay());
    //    query->bindValue(":dateStarted",  "NULL");
    //    query->bindValue(":dateFinished", "NULL");
    //    query->bindValue(":pagesHand","NULL");
    //    query->bindValue(":pagesWord","NULL");
    query->bindValue(":date_created", doc->getDateCreated());
    query->bindValue(":date_modified", doc->getDateModified());
    query->exec();

    anyError(query->lastError());

    idDocument = query->lastInsertId().toInt();

    return idDocument;
}

int DBH::updateDocument(Document *doc)
{
    int idDoc = -1;

    query->clear();
    query->prepare("UPDATE documents SET pagesDone= :pagesDone, pagesHand= :pagesHand, "
                   "pagesWord= :pagesWord, isPrinted= :isPrinted, deliveryDay= :deliveryDay"
                   " WHERE id=:id");

    query->bindValue(":id", doc->getID());
    query->bindValue(":pagesDone", doc->getPagesDone());
    query->bindValue(":pagesHand", doc->getPagesHand());
    query->bindValue(":pagesWord", doc->getPagesWord());
    query->bindValue(":isPrinted", doc->getIsPrinted());
    query->bindValue(":deliveryDay", doc->getDeliveryDay());

    query->exec();

    if (query->lastError().type() == QSqlError::NoError)
        idDoc = 1;
    else
        qCDebug(DDB) << query->lastError().type();

    return idDoc;
}

QList<Document *> DBH::getAvailableDocsList()
{
    QList<Document *> list;
    list.clear();
    Document *doc;

    query->clear();
    query->prepare("SELECT id,idClient,titleDoc FROM documents "
                   "WHERE dateStarted IS NULL ORDER BY titleDoc ASC ");
    query->exec();

    while (query->next())
    {
        doc = new Document();
        doc->setID(query->value("id").toInt());
        doc->setTitleDoc(query->value("titleDoc").toString());
        list << doc;
    }
    return list;
}

QList<Worker *> DBH::getWorkersList()
{
    QList<Worker *> list;
    list.clear();
    Worker *wk;

    query->prepare("SELECT id,fullName FROM workers WHERE isDisponible= :isDisponible");
    query->bindValue(":isDisponible", true);
    query->exec();

    while (query->next())
    {
        wk = new Worker();
        wk->setID(query->value("id").toInt());
        wk->setFullName(query->value("fullName").toString());
        list << wk;
    }

    query->clear();
    return list;
}

bool DBH::assignDoc2Worker(const QString &workerName, const QString &docTitle)
{
    int idWorker = -1;
    int idDoc = -1;

    qCDebug(DDB) << "Worker Name : " << workerName
                 << "doc Title : " << docTitle;   

    QSqlDatabase connection = QSqlDatabase::database();
    if(connection.isOpen() || connection.open())
    {

        connection.transaction();

        //! Get Both Indexes Document & Worker.
        query->clear();
        query->prepare("SELECT id FROM workers WHERE fullName=:fullName ");
        query->bindValue(":fullName", workerName);
        query->exec();
        if (query->next())
            idWorker = query->value("id").toInt();
        query->clear();

        query->prepare("SELECT id FROM documents WHERE titleDoc=:titleDoc ");
        query->bindValue(":titleDoc", docTitle);
        query->exec();
        if (query->next())
            idDoc = query->value("id").toInt();
        query->clear();

        query->prepare("UPDATE documents SET pagesDone=:pagesDone,dateStarted=:dateStarted,"
                       "idWorker =:idWorker WHERE id=:id ");

        query->bindValue(":pagesDone", 0);
        query->bindValue(":dateStarted", QDate::currentDate().toString("yyyy-MM-dd"));
        query->bindValue(":idWorker", idWorker);
        query->bindValue(":id", idDoc);
        query->exec();
        query->clear();

        query->prepare("UPDATE workers SET currentDocID=:currentDocID WHERE id=:id ");
        query->bindValue(":currentDocID", idDoc);
        query->bindValue(":id", idWorker);
        query->exec();
        query->clear();

        connection.commit();
    }
    return (idWorker != -1);
}

QList<Document *> DBH::getActiveDocsList()
{
    QList<Document *> list;
    list.clear();
    Document *doc;

    query->clear();
    query->prepare("SELECT id,idWorker,titleDoc,"
                   "totalPages,pagesDone,depositeDay,"
                   "deliveryDay FROM documents WHERE "
                   "dateStarted IS NOT NULL AND dateFinished IS NULL");

    query->exec();

    while (query->next())
    {
        doc = new Document();
        doc->setID(query->value("id").toInt());
        doc->setWriterID(query->value("idWorker").toInt());
        doc->setTitleDoc(query->value("titleDoc").toString());
        doc->setTotalPages(query->value("totalPages").toInt());
        doc->setPagesDone(query->value("pagesDone").toInt());
        doc->setDepositeDay(query->value("depositeDay").toString());
        doc->setDeliveryDay(query->value("deliveryDay").toString());
        // QDate::fromString(query->value("deliveryDay").toString(),"yyyy-MM-dd")
        list << doc;
    }
    return list;
}

QString DBH::getWorkerNameByID(int id)
{
    QString name;
    name.clear();


    query->prepare("SELECT fullName FROM workers WHERE id= :id ");
    query->bindValue(":id", id);
    query->exec();
    query->clear();

    if (query->next()) name = query->value("fullName").toString();

    return name;
}

QList<Document *> DBH::getArchivedDocsList()
{
    QList<Document *> list;
    list.clear();
    Document *doc;

    query->prepare("SELECT id,idClient,titleDoc,pagesDone "
                   "FROM documents WHERE dateFinished IS NOT NULL "); //
    // query->bindValue(":dateFinished",-1);
    query->exec();

    while (query->next())
    {
        doc = new Document();
        doc->setID(query->value("id").toInt());
        doc->setOwnerID(query->value("idClient").toInt());
        doc->setTitleDoc(query->value("titleDoc").toString());
        doc->setPagesDone(query->value("pagesDone").toInt());
        list << doc;
    }
    query->clear();
    return list;
}

QString DBH::getClientNameByID(int id)
{
    QString clientName;
    clientName.clear();

    query->prepare("SELECT fullname FROM clients WHERE id=:id");
    query->bindValue(":fullname", id);
    query->exec();

    if (query->next())
        clientName = query->value("fullname").toString();
    query->clear();

    return clientName;
}

int DBH::updateWorker(Worker *data)
{
    int rowsUpdated = -1;

    query->prepare("UPDATE workers SET fullName=:fullName,"
                   "phoneNumber=:phoneNumber,email=:email,"
                   "remarke=:remarke,isDisponible=:isDisponible WHERE "
                   "id=:id");

    query->bindValue(":id", data->getID());
    query->bindValue(":fullName", data->getFullName());
    query->bindValue(":phoneNumber", data->getPhoneNumber());
    query->bindValue(":email", data->getEmail());
    query->bindValue(":remarke", data->getRemarke());
    query->bindValue(":isDisponible", data->getIsDisponible());

    query->exec();
    rowsUpdated = query->numRowsAffected();
    query->clear();

    return rowsUpdated;
}

int DBH::addWorker(Worker *data)
{
    int idWorker = -1;

    query->prepare("INSERT INTO workers(fullName,phoneNumber,email,remarke,isDisponible,currentDocID,date_created,date_modified) "
                               "VALUES(:fullName,:phoneNumber,:email,:remarke,:isDisponible,NULL,:date_created,:date_modified)");

    query->bindValue(":fullName", data->getFullName());
    query->bindValue(":phoneNumber", data->getPhoneNumber());
    query->bindValue(":email", data->getEmail());
    query->bindValue(":remarke", data->getRemarke());
    query->bindValue(":isDisponible", data->getIsDisponible());
    query->bindValue(":date_created", data->getDateCreated());
    query->bindValue(":date_modified", data->getDateModified());
    query->exec();

    anyError(query->lastError());
    idWorker = query->lastInsertId().toInt();
    query->clear();

    return idWorker;
}

int DBH::deleteWorkerbyID(int id)
{
    int rowDeleted = -1;
    int currentDocID = 0;

    QSqlDatabase connection = QSqlDatabase::database();
    if(connection.isOpen() || connection.open())
    {

        connection.transaction();

        query->prepare("SELECT currentDocID FROM workers WHERE id= :id");
        query->bindValue(":id", id);
        query->exec();
        if (query->next())
            currentDocID = query->value("currentDocID").toInt();
        query->clear();

        query->prepare("UPDATE documents SET idWorker=:idWorker WHERE id=:id "
                       "AND totalPages<>pagesDone");
        query->bindValue(":idWorker", -1);
        query->bindValue(":id", currentDocID);
        query->exec();
        query->clear();

        query->prepare("SELECT id FROM documents WHERE idWorker= :idWorker");
        query->bindValue(":idWorker", id);
        query->exec();

        if (query->size() > 0)
        {
            query->clear();
            query->prepare("UPDATE workers SET isDisponible='NO' WHERE id=:id");
            query->bindValue(":id", id);
            query->exec();
        }
        else
        {
            query->clear();
            query->prepare("DELETE FROM workers WHERE id= :id");
            query->bindValue(":id", id);
            query->exec();
            rowDeleted = query->numRowsAffected();
        }
        connection.commit();
    }

    return rowDeleted;
}

int DBH::updateAdminName(const QString &name)
{
    int rowUpdated = -1;
    SimpleCrypt crypto(Q_UINT64_C(0x16af28db99bbca1f));

    query->prepare("UPDATE admin SET username= :username WHERE app_name='yassin' ");
    query->bindValue(":username", crypto.encryptToString(name));
    query->exec();
    rowUpdated = query->numRowsAffected();
    query->clear();

    return rowUpdated;
}

int DBH::updateAdminPassword(const QString &password)
{
    int rowUpdated = -1;

    SimpleCrypt crypto(Q_UINT64_C(0x16af28db99bbca1f));

    query->prepare("UPDATE admin SET password= :password WHERE app_name='yassin' ");
    query->bindValue(":password", crypto.encryptToString(password));
    query->exec();
    rowUpdated = query->numRowsAffected();
    query->clear();

    return rowUpdated;
}

int DBH::updateAdminAddress(const QString &address)
{
    int rowUpdated = -1;

    query->prepare("UPDATE admin SET localAdress= :localAdress WHERE app_name='yassin' ");
    query->bindValue(":localAdress", address);
    query->exec();
    rowUpdated = query->numRowsAffected();
    query->clear();

    return rowUpdated;
}

int DBH::updateAdminPhone(const QString &phone)
{
    int rowUpdated = -1;

    query->prepare("UPDATE admin SET phoneNumber= :phoneNumber WHERE app_name='yassin' ");
    query->bindValue(":phoneNumber", phone);
    query->exec();
    query->clear();
    rowUpdated = query->numRowsAffected();

    return rowUpdated;
}

int DBH::cancelAssignment(int idDoc, int idWorker)
{
    int rowUpdated = -1;

    QSqlDatabase connection = QSqlDatabase::database();
    if(connection.isOpen() || connection.open())
    {

        connection.transaction();

        query->prepare("UPDATE documents SET idWorker = NULL,dateStarted = NULL WHERE id=:id");
        query->bindValue(":id", idDoc);
        query->exec();
        rowUpdated = query->numRowsAffected();
        query->clear();

        query->prepare("UPDATE workers SET currentDocID = NULL WHERE id= :id");
        query->bindValue(":id", idWorker);
        query->exec();
        rowUpdated += query->numRowsAffected();
        query->clear();

        connection.commit();
    }
    return rowUpdated;
}

int DBH::moveToArchive(int idDoc)
{
    int rowUpdated = -1;

    query->prepare("UPDATE documents SET dateFinished=:dateFinished WHERE id=:id");
    query->bindValue(":dateFinished", QDate::currentDate().toString("yyyy-MM-dd")); // FIXME :
    query->bindValue(":id", idDoc);
    query->exec();
    rowUpdated = query->numRowsAffected();
    query->clear();

    return rowUpdated;
}

int DBH::removeFromArchive(int idDoc)
{
    int rowUpdated = -1;

    query->prepare("UPDATE documents SET dateFinished = NULL WHERE id= :id");
    query->bindValue(":id", idDoc);
    query->exec();
    rowUpdated = query->numRowsAffected();
    query->clear();

    return rowUpdated;
}

Client *DBH::getClientAndDocByID(int id)
{
    Client *data = nullptr;
    Document *doc = nullptr;
    int idClient = -1;

    QSqlDatabase connection = QSqlDatabase::database();
    if(connection.isOpen() || connection.open())
    {
        connection.transaction();

        query->prepare("SELECT idClient,titleDoc,totalPages,"
                       "pagesDone,isPrinted,dateFinished "
                       "FROM documents WHERE id= :id");
        query->bindValue(":id", id);
        query->exec();
        if (query->next())
        {
            doc = new Document();
            idClient = query->value("idClient").toInt();
            doc->setTitleDoc(query->value("titleDoc").toString());
            doc->setTotalPages(query->value("totalPages").toInt());
            doc->setPagesDone(query->value("pagesDone").toInt());
            doc->setIsPrinted(query->value("isPrinted").toBool());
            doc->setDateFinished(query->value("dateFinished").toString());
        }

        query->clear();
        query->prepare("SELECT fullname,price,pricePaid FROM clients WHERE id=:id");
        query->bindValue(":id", idClient);
        query->exec();

        if (query->next())
        {
            data = new Client();
            data->setID(idClient);
            data->setFullname(query->value("fullname").toString());
            data->setPrice(query->value("price").toInt());
            data->setPricePaid(query->value("pricePaid").toInt());
        }
        data->setDocument(doc);
        query->clear();

        connection.commit();
    }
    return data;
}
