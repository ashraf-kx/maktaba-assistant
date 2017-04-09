#ifndef DIALOG_H
#define DIALOG_H

#include <QtWidgets/QDialog>

namespace Ui {
class Dialog;
}

class Dialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit Dialog(QWidget *parent = 0);
    ~Dialog();
    void setMessage(const QString &msg, const QString &typeM);
private:
    Ui::Dialog *ui;
};

#endif // DIALOG_H
