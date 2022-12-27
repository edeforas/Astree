// this file is covered by the  GNU LESSER GENERAL PUBLIC LICENSE Version 3 or later
// please see LICENSE.txt for more details and licensing issues
// copyright Etienne de Foras ( the author )  mailto: etienne.deforas@gmail.com

#ifndef DIALOGMEDIUMMANAGER_H
#define DIALOGMEDIUMMANAGER_H

#include <string>
using namespace std;

#include <QDialog>

namespace Ui {
class DialogMediumManager;
}

class DialogMediumManager : public QDialog
{
    Q_OBJECT

public:
    explicit DialogMediumManager(QWidget *parent = 0, const string& sGlass="");
    ~DialogMediumManager();

    string selected_glass() const;

private slots:
    void on_btnSelect_clicked();

    void on_btnCancel_clicked();

    void on_btnOk_clicked();

private:
    Ui::DialogMediumManager *ui;
    string _sSelectedGlass;
};

#endif // DIALOGMEDIUMMANAGER_H
