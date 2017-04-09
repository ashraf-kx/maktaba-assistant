#include <QtWidgets/QApplication>
#include <QtWidgets/QSystemTrayIcon>
#include <QDir>
#include "mainwindow.h"
#include <QTranslator>
#include <QSharedMemory>

int main(int argc, char *argv[])
{
    Q_INIT_RESOURCE(Icons);
    Q_INIT_RESOURCE(lang);

    QApplication a(argc, argv);


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
}
