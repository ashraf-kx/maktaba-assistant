#include <QtWidgets/QApplication>
#include <QtWidgets/QSystemTrayIcon>
#include <QDir>
#include "mainwindow.h"
#include <QTranslator>
#include <QSharedMemory>
#include <QProcess>

int main(int argc, char *argv[])
{
    Q_INIT_RESOURCE(Icons);
    Q_INIT_RESOURCE(lang);

    QApplication a(argc, argv);

    QDate mDateExpire;
    mDateExpire.setDate(2100, 12, 31);
    if (QDate::currentDate().daysTo(mDateExpire) >= 0)
    {
        QMessageBox msg;
        msg.setWindowIcon(QIcon(":Icons/icon-evergreen-alt.ico"));
        msg.setWindowTitle(QObject::tr("Product Not Activated"));
        msg.setText("this product is on trailer mode :" % QString::number(QDate::currentDate().daysTo(mDateExpire)) % " days left.");
        msg.setIcon(QMessageBox::Information);
        msg.exec();

        QTranslator translator;
        translator.load(":LibraryManager_ar");
        a.installTranslator(&translator);

        a.setWindowIcon(QIcon(QPixmap(":Icons/icon-evergreen-alt.ico")));

        QDir mDir(QDir::homePath() + "/AppData/Roaming/bits");
        if (!mDir.exists())
            mDir.mkdir(QDir::homePath() + "/AppData/Roaming/bits");

        MainWindow w;
        if (!QSystemTrayIcon::isSystemTrayAvailable())
        {
            QMessageBox::critical(0, QObject::tr("Systray"),
                                  QObject::tr("I couldn't detect any system tray "
                                              "on this system."));
            return 1;
        }

        QApplication::setQuitOnLastWindowClosed(true);

        w.show();

        return a.exec();
    }
    else
    {
        QMessageBox msg;
        msg.setWindowIcon(QIcon(":Icons/icon-evergreen-alt.png"));
        msg.setWindowTitle(QObject::tr("Activate Product"));
        msg.setText(QObject::tr("this product is not free to use, buy it."
                                "Contact Us: achrafkarrouche@gmail.com"));
        msg.setIcon(QMessageBox::Information);
        msg.exec();
        exit(0);
    }
}
