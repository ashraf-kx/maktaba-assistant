#include "db.h"
#include "simplecrypt.h"
#include <QDateTime>
#include <QtWidgets/QMessageBox>

DB::DB(QObject *parent) : QObject(parent)
{
    const static QString T_Lang = "(id INTEGER PRIMARY KEY AUTOINCREMENT,"
                                  "languageName VARCHAR(20),"
                                  "abbrev VARCHAR(4))";

    const static QString workersTable = "(id INTEGER PRIMARY KEY AUTOINCREMENT,"
                                     "fullName VARCHAR(50),"
                                     "phoneNumber VARCHAR(10),"
                                     "email varchar(100),"
                                     "remarke VARCHAR(250),"
                                     "isDisponible VARCHAR(15),"
                                     "currentDocID INTEGER,"
                                     "date_created TIMESTAMP,"
                                     "date_modified TIMESTAMP)";

    const static QString documentsTable = "(id INTEGER PRIMARY KEY AUTOINCREMENT,"
                                       "idClient INTEGER,"
                                       "idWorker INTEGER,"
                                       "titleDoc VARCHAR(250),"
                                       "typeDoc VARCHAR(15),"
                                       "fontFamily VARCHAR(50),"
                                       "fontSize INTEGER,"
                                       "printingColor INTEGER,"
                                       "isPrinted VARCHAR(20),"
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

    const static QString adminTable = "(id INTEGER PRIMARY KEY AUTOINCREMENT,"
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

    const static QString clientsTable = "(id INTEGER PRIMARY KEY AUTOINCREMENT,"
                                     "fullname VARCHAR(100),"
                                     "phoneNumber VARCHAR(14),"
                                     "firstEmail varchar(100),"
                                     "isDeliveredByMail VARCHAR(3),"
                                     "remarke VARCHAR(600),"
                                     "payement_state VARCHAR(50),"
                                     "price INTEGER,"
                                     "pricePaid INTEGER,"
                                     "date_created TIMESTAMP,"
                                     "date_modified TIMESTAMP)";

    QSqlDatabase connection = QSqlDatabase::addDatabase("QSQLITE");
#ifdef Q_OS_LINUX
    connection.setDatabaseName(QDir::homePath() + "/" + "libraryAssistant.db");
#else
    connection.setDatabaseName(QDir::homePath()+"/AppData/Roaming/km2/"+"libraryAssistant.db");
#endif

    qInfo() << "Driver used: " << connection.driverName() << Qt::endl
            << "support transactions: " << connection.driver()->hasFeature(QSqlDriver::Transactions);

    if (connection.open())
    {
        qInfo() << "default database connection opened.";
        connection.transaction();
        const QStringList dbTables = connection.tables();
        QSqlQuery *query = new QSqlQuery(connection);

        if (!dbTables.contains("workers"))
        {
            query->exec("CREATE TABLE IF NOT EXISTS 'workers' " % workersTable) ? qInfo() << "Table 'workers' created." << Qt::endl
                                                                             : qDebug() << "Creation of table 'workers' failed.";
            query->clear();
        }
        else
            qInfo() << "Table 'workers' already exist.";

        if (!dbTables.contains("admin"))
        {
            if (query->exec("CREATE TABLE IF NOT EXISTS 'admin' " % adminTable))
            {
                qInfo() << "Table 'admin' created." << Qt::endl;
                SimpleCrypt crypto(Q_UINT64_C(0x16af28db99bbca1f));
                QString name = crypto.encryptToString(QString("admin"));

                query->clear();
                query->prepare("INSERT INTO admin (id,app_name,username,password,lastLogin,"
                               "timeLogs,phoneNumber,date_created,date_modified)"
                               "VALUES(NULL,:app_name,:username,:password,:lastLogin,"
                               ":timeLogs,:phoneNumber,:date_created,:date_modified)"
                               " ");

                query->bindValue(":app_name", "yassin");
                query->bindValue(":username", name);
                query->bindValue(":password", name);
                query->bindValue(":lastLogin", -1);
                query->bindValue(":phoneNumber", "----------");
                query->bindValue(":timeLogs", 0);
                query->bindValue(":date_created", QDateTime::currentDateTimeUtc().toTime_t());
                query->bindValue(":date_modified", QDateTime::currentDateTimeUtc().toTime_t());
                query->exec();
                query->clear();
                qInfo() << "Default user has been created." << Qt::endl;
            }
            else
                qDebug() << "Creation of table 'admin' failed.";
        }
        else
            qInfo() << "Table 'admin' already exist.";

        if (!dbTables.contains("documents"))
        {
            query->exec("CREATE TABLE IF NOT EXISTS 'documents' " % documentsTable) ? qInfo() << "Table 'documents' created." << Qt::endl
                                                                                 : qDebug() << "Creation of table 'documents' failed." << Qt::endl;
            query->clear();
        }
        else
            qInfo() << "Table 'documents' already exist.";

        if (!dbTables.contains("clients"))
        {
            query->exec("CREATE TABLE IF NOT EXISTS 'clients' " % clientsTable) ? qInfo() << "Table 'clients' created."
                                                                             : qDebug() << "Creation of table 'clients' failed.";
            query->clear();
        }
        else
            qInfo() << "Table 'clients' already exist.";
        query->clear();
    }
    else
        qDebug() << "couldn't open the default database connection.";

    connection.commit();
    connection.close();
}
