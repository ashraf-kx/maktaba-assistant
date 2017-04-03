#ifndef TOPNAV_H
#define TOPNAV_H

#include <QWidget>
#include <QPushButton>

namespace Ui {
class topNav;
}

class topNav : public QWidget
{
    Q_OBJECT
    
public:
    explicit topNav(QWidget *parent = 0);
    QPushButton* getBTClient();
    QPushButton* getBTWorker();
    QPushButton* getBTDocs();
    QPushButton* getBTHelp();
    ~topNav();

    QPushButton *getBTLogOut();

public slots:
    void changeStyleClient();
    void changeStyleWorker();
    void changeStyleBooks();
private:
    Ui::topNav *ui;
};

#endif // TOPNAV_H
