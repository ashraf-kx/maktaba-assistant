#include "db.h"
#include "simplecrypt.h"
#include <QDateTime>
#include <QMessageBox>


DB::DB(QObject *parent) :
    QObject(parent)
{
    const static QString T_Lang = "(id INTEGER PRIMARY KEY   AUTOINCREMENT,"
                           "languageName VARCHAR(20),"
                           "abbrev VARCHAR(4))";

    const static QString T_Workers = "(id INTEGER PRIMARY KEY   AUTOINCREMENT,"
                           "fullName VARCHAR(50),"
                           "phoneNumber VARCHAR(10),"
                           "email varchar(100),"
                           "remarke VARCHAR(250),"
                           "isDisponible VARCHAR(3),"
                           "currentDocID INTEGER,"
                           "date_created TIMESTAMP,"
                           "date_modified TIMESTAMP)";

    const static QString T_Documents = "(id INTEGER PRIMARY KEY   AUTOINCREMENT,"
                           "idClient INTEGER,"
                           "idWorker INTEGER,"
                           "titleDoc VARCHAR(250),"
                           "typeDoc VARCHAR(15),"
                           "fontFamily VARCHAR(50),"
                           "fontSize INTEGER,"
                           "printingColor INTEGER,"
                           "isPrinted INTEGER,"
                           "languageDoc VARCHAR(30),"
                           "totalPages INTEGER,"
                           "pagesDone INTEGER,"
                           "depositeDay DATE,"
                           "deliveryDay DATE,"
                           "dateStarted DATE,"
                           "dateFinished DATE,"
                           "date_created TIMESTAMP,"
                           "date_modified TIMESTAMP)";

    const static QString T_Admin = "(id INTEGER PRIMARY KEY AUTOINCREMENT,"
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

    const static QString T_Clients = "(id INTEGER PRIMARY KEY AUTOINCREMENT,"
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


            this->DBH = QSqlDatabase::addDatabase("QSQLITE");
            this->DBH.setDatabaseName(QDir::homePath()+"/AppData/Roaming/bits/"+"BYASS.db");
            this->DBH.setPassword("bitProjects");
            this->DBH.setUserName("neverAsk@4Pass");

            if(this->DBH.open()) qDebug()<<"default cnxn opened."<<endl;
            this->DBH.transaction();
            // create the list tables of the schema.
            if(createTable("workers",T_Workers)) qDebug()<<"Table workers OK."<<endl;
            if(!this->DBH.tables().contains("admin"))
            {   createTable("admin",T_Admin);
                SimpleCrypt crypto(Q_UINT64_C(0x16af28db99bbca1f));
                QString name= crypto.encryptToString(QString("admin"));

                qDebug()<<"table Admin OK."<<endl;
                QSqlQuery *query =new QSqlQuery(this->DBH);
                query->prepare("INSERT INTO admin (id,app_name,username,password,lastLogin,"
                              "timeLogs,phoneNumber,date_created,date_modified)"
                              "VALUES(NULL,:app_name,:username,:password,:lastLogin,"
                              ":timeLogs,:phoneNumber,:date_created,:date_modified)"
                              " ");

                query->bindValue(":app_name","yassin");
                query->bindValue(":username",name);
                query->bindValue(":password",name);
                query->bindValue(":lastLogin", -1 );
                query->bindValue(":phoneNumber", "----------");
                query->bindValue(":timeLogs", 0);
                query->bindValue(":date_created",QDateTime::currentDateTime().toTime_t());
                query->bindValue(":date_modified",QDateTime::currentDateTime().toTime_t());
                query->exec();
                qDebug()<<"Default Admin OK."<<endl;
            }
            if(createTable("documents",T_Documents)) qDebug()<<"Table Documents OK."<<endl;
            if(createTable("clients",T_Clients))     qDebug()<<"Table Clients OK."<<endl;
            this->DBH.commit();
}


bool DB::createTable(const QString &nameTable,const QString &tableQueryStructure)
{
    QSqlQuery *query = new QSqlQuery(this->DBH);
    return query->exec("CREATE TABLE IF NOT EXISTS '"+nameTable+"' "+tableQueryStructure);
}


