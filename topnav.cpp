#include "topnav.h"
#include "ui_topnav.h"

topNav::topNav(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::topNav)
{
    ui->setupUi(this);

    changeStyleClient();

    connect(ui->BT_Clients,SIGNAL(clicked()),this,SLOT(changeStyleClient()));
    connect(ui->BT_Workers,SIGNAL(clicked()),this,SLOT(changeStyleWorker()));
    connect(ui->BT_Books,SIGNAL(clicked()),this,SLOT(changeStyleBooks()));
}

void topNav::changeStyleClient()
{
    ui->BT_Clients->setStyleSheet("QPushButton"
                                  "{"
                                      "font-size: 15px;"
                                      "font-family: Ubuntu;"
                                      "color: #FFFFFF;"
                                      "white-space: nowrap;"
                                      "background-color: #3F51B5;"
                                      "border-radius: 0px;"
                                      "border: 1px solid #3F51B5 ;"
                                      "border-bottom: 2px solid #FF4081;"
                                  "}"
                                  "QPushButton:hover"
                                  "{"
                                   "background-color: #03A9F4;"
                                   "border-top: 1px solid  #03A9F4 ;"
                                   "border-left: 1px solid  #03A9F4 ;"
                                   "border-right: 1px solid  #03A9F4 ;"
                                   "border-bottom: 2px solid #FF4081;"
                                   "color: #ffffff;"
                                  "}");

    ui->BT_Workers->setStyleSheet("QPushButton"
                                  "{"
                                      "font-size: 15px;"
                                      "font-family: Ubuntu;"
                                      "color: #9E9E9E;"
                                      "white-space: nowrap;"
                                      "background-color: #3F51B5;"
                                      "border-radius: 0px;"
                                      "border: 1px solid #3F51B5 ;"
                                  "}"
                                  "QPushButton:hover"
                                  "{"
                                   "background-color: #03A9F4;"
                                   "border-top: 1px solid  #03A9F4 ;"
                                   "border-left: 1px solid  #03A9F4 ;"
                                   "border-right: 1px solid  #03A9F4 ;"
                                   "border-bottom: 2px solid #FF4081;"
                                   "color: #ffffff;"
                                  "}");
    ui->BT_Books->setStyleSheet("QPushButton"
                                "{"
                                    "font-size: 15px;"
                                    "font-family: Ubuntu;"
                                    "color: #9E9E9E;"
                                    "white-space: nowrap;"
                                    "background-color: #3F51B5;"
                                    "border-radius: 0px;"
                                    "border: 1px solid #3F51B5 ;"
                                "}"
                                "QPushButton:hover"
                                "{"
                                 "background-color: #03A9F4;"
                                 "border-top: 1px solid  #03A9F4 ;"
                                 "border-left: 1px solid  #03A9F4 ;"
                                 "border-right: 1px solid  #03A9F4 ;"
                                 "border-bottom: 2px solid #FF4081;"
                                 "color: #ffffff;"
                                "}");

}

void topNav::changeStyleWorker()
{
    ui->BT_Workers->setStyleSheet("QPushButton"
                                  "{"
                                      "font-size: 15px;"
                                      "font-family: Ubuntu;"
                                      "color: #FFFFFF;"
                                      "white-space: nowrap;"
                                      "background-color: #3F51B5;"
                                      "border-radius: 0px;"
                                      "border: 1px solid #3F51B5 ;"
                                      "border-bottom: 2px solid #FF4081;"
                                  "}"
                                  "QPushButton:hover"
                                  "{"
                                   "background-color: #03A9F4;"
                                   "border-top: 1px solid  #03A9F4 ;"
                                   "border-left: 1px solid  #03A9F4 ;"
                                   "border-right: 1px solid  #03A9F4 ;"
                                   "border-bottom: 2px solid #FF4081;"
                                   "color: #ffffff;"
                                  "}");

    ui->BT_Clients->setStyleSheet("QPushButton"
                                  "{"
                                      "font-size: 15px;"
                                      "font-family: Ubuntu;"
                                      "color: #9E9E9E;"
                                      "white-space: nowrap;"
                                      "background-color: #3F51B5;"
                                      "border-radius: 0px;"
                                      "border: 1px solid #3F51B5 ;"
                                  "}"
                                  "QPushButton:hover"
                                  "{"
                                   "background-color: #03A9F4;"
                                   "border-top: 1px solid  #03A9F4 ;"
                                   "border-left: 1px solid  #03A9F4 ;"
                                   "border-right: 1px solid  #03A9F4 ;"
                                   "border-bottom: 2px solid #FF4081;"
                                   "color: #ffffff;"
                                  "}");
    ui->BT_Books->setStyleSheet("QPushButton"
                                "{"
                                    "font-size: 15px;"
                                    "font-family: Ubuntu;"
                                    "color: #9E9E9E;"
                                    "white-space: nowrap;"
                                    "background-color: #3F51B5;"
                                    "border-radius: 0px;"
                                    "border: 1px solid #3F51B5 ;"
                                "}"
                                "QPushButton:hover"
                                "{"
                                 "background-color: #03A9F4;"
                                 "border-top: 1px solid  #03A9F4 ;"
                                 "border-left: 1px solid  #03A9F4 ;"
                                 "border-right: 1px solid  #03A9F4 ;"
                                 "border-bottom: 2px solid #FF4081;"
                                 "color: #ffffff;"
                                "}");
}

void topNav::changeStyleBooks()
{
    ui->BT_Books->setStyleSheet("QPushButton"
                                  "{"
                                      "font-size: 15px;"
                                      "font-family: Ubuntu;"
                                      "color: #FFFFFF;"
                                      "white-space: nowrap;"
                                      "background-color: #3F51B5;"
                                      "border-radius: 0px;"
                                      "border: 1px solid #3F51B5 ;"
                                      "border-bottom: 2px solid #FF4081;"
                                  "}"
                                  "QPushButton:hover"
                                  "{"
                                   "background-color: #03A9F4;"
                                   "border-top: 1px solid  #03A9F4 ;"
                                   "border-left: 1px solid  #03A9F4 ;"
                                   "border-right: 1px solid  #03A9F4 ;"
                                   "border-bottom: 2px solid #FF4081;"
                                   "color: #ffffff;"
                                  "}");

    ui->BT_Workers->setStyleSheet("QPushButton"
                                  "{"
                                      "font-size: 15px;"
                                      "font-family: Ubuntu;"
                                      "color: #9E9E9E;"
                                      "white-space: nowrap;"
                                      "background-color: #3F51B5;"
                                      "border-radius: 0px;"
                                      "border: 1px solid #3F51B5 ;"
                                  "}"
                                  "QPushButton:hover"
                                  "{"
                                   "background-color: #03A9F4;"
                                   "border-top: 1px solid  #03A9F4 ;"
                                   "border-left: 1px solid  #03A9F4 ;"
                                   "border-right: 1px solid  #03A9F4 ;"
                                   "border-bottom: 2px solid #FF4081;"
                                   "color: #ffffff;"
                                  "}");
    ui->BT_Clients->setStyleSheet("QPushButton"
                                "{"
                                    "font-size: 15px;"
                                    "font-family: Ubuntu;"
                                    "color: #9E9E9E;"
                                    "white-space: nowrap;"
                                    "background-color: #3F51B5;"
                                    "border-radius: 0px;"
                                    "border: 1px solid #3F51B5 ;"
                                "}"
                                "QPushButton:hover"
                                "{"
                                 "background-color: #03A9F4;"
                                 "border-top: 1px solid  #03A9F4 ;"
                                 "border-left: 1px solid  #03A9F4 ;"
                                 "border-right: 1px solid  #03A9F4 ;"
                                 "border-bottom: 2px solid #FF4081;"
                                 "color: #ffffff;"
                                "}");
}


QPushButton *topNav::getBTClient()
{
    return ui->BT_Clients;
}

QPushButton *topNav::getBTWorker()
{
    return ui->BT_Workers;
}

QPushButton *topNav::getBTDocs()
{
    return ui->BT_Books;
}

QPushButton *topNav::getBTLogOut()
{
    return ui->Bt_logOut;
}

topNav::~topNav()
{
    delete ui;
}


