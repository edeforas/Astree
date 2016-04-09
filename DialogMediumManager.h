#ifndef DIALOGMEDIUMMANAGER_H
#define DIALOGMEDIUMMANAGER_H

#include <QDialog>

namespace Ui {
class DialogMediumManager;
}

class DialogMediumManager : public QDialog
{
    Q_OBJECT

public:
    explicit DialogMediumManager(QWidget *parent = 0);
    ~DialogMediumManager();

private slots:
    void on_pushButton_3_clicked();

private:
    Ui::DialogMediumManager *ui;
};

#endif // DIALOGMEDIUMMANAGER_H
