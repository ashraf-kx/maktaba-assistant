#ifndef DBH_H
#define DBH_H

#include <classes.h>
#include <QObject>
Q_DECLARE_LOGGING_CATEGORY(DDB)

class DBH : public QSqlDatabase
{
    // Q_OBJECT
private:
    QString    connectionName;
    QSqlQuery *query;

public:
    DBH();
    DBH(const QString& name);

    QSqlDatabase getCnx();
    void mTransaction();
    void mCommit();
    void mRemoveDatabase(const QString& connectionName);
    int  createTable(const QString &nameTable, const QString &tableQueryStructure);

    void createEmptyDB();
    bool attemptLogin(const QString &username, const QString &password);

    int addClient(Client *data);
    int updateClient(Client *data);
    int deleteClientByID(int idClient);

    int addDocument(int idClient, Document *doc);
    int updateDocument(Document* doc);

    QList<Worker *>   getWorkersList();
    QList<Document *> getAvailableDocsList();
    bool assignDoc2Worker(const QString &workerName, const QString &docTitle);
    QString getWorkerNameByID(int id);
    QList<Document *> getActiveDocsList();
    QList<Document *> getArchivedDocsList();
    QString getClientNameByID(int id);
    int updateWorker(Worker *data);
    int addWorker(Worker *data);
    int deleteWorkerbyID(int id);

    int updateAdminName(const QString &name);
    int updateAdminPassword(const QString &password);
    int updateAdminAddress(const QString &address);
    int updateAdminPhone(const QString &phone);
    void anyError(QSqlError e);
    int cancelAssignment(int idDoc, int idWorker);
    int moveToArchive(int idDoc);
    int removeFromArchive(int idDoc);
    Client *getClientAndDocByID(int id);
};

#endif // DBH_H
