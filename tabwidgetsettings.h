#ifndef TABWIDGETSETTINGS_H
#define TABWIDGETSETTINGS_H

#include <QtWidgets/QTabWidget>
#include <QSqlError>
#include <QSqlQuery>
#include <QDebug>
#include <QSqlDatabase>
#include <QDateTime>
#include <QDir>
#include <simplecrypt.h>
#include "toast.h"

namespace Ui {
class TabWidgetSettings;
}

class TabWidgetSettings : public QTabWidget
{
    Q_OBJECT
    
public:
    explicit TabWidgetSettings(QWidget *parent = 0);
    ~TabWidgetSettings();

public slots:
    void updateAdminName();
    void updatePassword();
    void updateAdminAddress();
    void updateAdminPhone();

private:
    Ui::TabWidgetSettings *ui;
    QSqlDatabase DBH;
    QSqlQuery    *query;
    Toast        *mToast;
};

#endif // TABWIDGETSETTINGS_H
