#ifndef DIALOGREVERTDESIGN_H
#define DIALOGREVERTDESIGN_H

#include <QDialog>

namespace Ui {
class DialogRevertDesign;
}

class DialogRevertDesign : public QDialog
{
    Q_OBJECT

public:
    explicit DialogRevertDesign(int iNbSurfaces,QWidget *parent = nullptr);
    ~DialogRevertDesign();

	int first_surface();
	int last_surface();

private slots:

    void on_btnOk_clicked();

    void on_btnCancel_clicked();

private:
    Ui::DialogRevertDesign *ui;
};

#endif // DIALOGREVERTDESIGN_H
