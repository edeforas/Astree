#ifndef DIALOGSCALEDEVICE_H
#define DIALOGSCALEDEVICE_H

#include <QDialog>

namespace Ui {
class DialogScaleDevice;
}

class DialogScaleDevice : public QDialog
{
    Q_OBJECT

public:
    explicit DialogScaleDevice(QWidget *parent = 0);
    ~DialogScaleDevice();

    double get_scale();
    bool get_scale_diameter();
    bool get_scale_focal();

private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_rbFactor_clicked();

    void on_rbPercentage_clicked();

    void on_rbSizeChange_clicked();

private:
    Ui::DialogScaleDevice *ui;

    double _dScale;
    bool _bScaleDiameter;
    bool _bScaleFocal;
};

#endif // DIALOGSCALEDEVICE_H
