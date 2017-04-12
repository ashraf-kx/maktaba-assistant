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

    QString serialKey="ProcessorId       \r\r\nBFEBFBFF0001067A  \r\r\n\r\r\n";
    QProcess process;
    process.setWorkingDirectory(QDir::homePath());
    process.start("wmic cpu get ProcessorId"); // CPUID. wmic cpu get ProcessorId
    process.waitForBytesWritten();             // DISKID .wmic diskdrive get SerialNumber
    process.waitForFinished();

    if(serialKey == QString(process.readAll()))
    {
        QDate mDateExpire;
        mDateExpire.setDate(2017,05,05);
        if(QDate::currentDate().daysTo(mDateExpire) >= 0 )
        {
            QMessageBox msg;
            msg.setWindowIcon(QIcon("://Icons/icon-evergreen-alt.png"));
            msg.setWindowTitle(QObject::tr("Product Not Activated"));
            msg.setText("this product is on trailer mode, days left "
                         +QString::number(QDate::currentDate().daysTo(mDateExpire)) );
            msg.setIcon( QMessageBox::Information );
            msg.exec();

        QSharedMemory shared("62d60669-bb94-4a94-88bb-b964890a7e04");

              if( !shared.create( 512, QSharedMemory::ReadWrite) )
              {
                // For a GUI application, replace this by :
                QMessageBox msgBox;
                msgBox.setText( QObject::tr("Can't start more than one instance of the application.") );
                msgBox.setIcon( QMessageBox::Critical );
                msgBox.exec();

                // qWarning() << "Can't start more than one instance of the application.";

                exit(0);
              }

            QTranslator translator;
            translator.load(":biblioYass_ar");
            a.installTranslator(&translator);

        a.setWindowIcon(QIcon(QPixmap(":Icons/icon-evergreen-alt.png")));

        QDir mDir(QDir::homePath()+"/AppData/Roaming/bits");
        if(!mDir.exists())
            mDir.mkdir(QDir::homePath()+"/AppData/Roaming/bits");

        MainWindow w;
        if (!QSystemTrayIcon::isSystemTrayAvailable()) {
            QMessageBox::critical(0, QObject::tr("Systray"),
                                  QObject::tr("I couldn't detect any system tray "
                                              "on this system."));
            return 1;
        }

        QApplication::setQuitOnLastWindowClosed(true);

        w.show();

        return a.exec();
        }else{
            QMessageBox msg;
            msg.setWindowIcon(QIcon("://Icons/icon-evergreen-alt.png"));
            msg.setWindowTitle(QObject::tr("Activate Product"));
            msg.setText( QObject::tr("this product is not free to use, buy it."
                                     "Contact Us: scos2017@gmail.com") );
            msg.setIcon( QMessageBox::Information );
            msg.exec();
            exit(0);
        }
    }else
    {
        QMessageBox msg;
        msg.setWindowIcon(QIcon("://Icons/icon-evergreen-alt.png"));
        msg.setWindowTitle(QObject::tr("Activate Product"));
        msg.setText( QObject::tr("this product is not free to use, buy it."
                                 "Contact Us: scos2017@gmail.com") );
        msg.setIcon( QMessageBox::Information );
        msg.exec();
        exit(0);
    }
}
