#ifndef DB_H
#define DB_H

#include <QObject>
#include <QtWidgets/QMessageBox>
#include <QSqlError>
#include <QSqlQuery>
#include <QDebug>
#include <QSqlDatabase>
#include <QDateTime>
#include <QDir>
#include <QtSql>

class DB : public QObject
{
    Q_OBJECT
private:
public:
    explicit DB(QObject *parent = 0);
};

#endif // DB_H
