#ifndef DOCKOPTIMIZER_H
#define DOCKOPTIMIZER_H

#include <QDockWidget>

#include "DeviceOptimizer.h"

namespace Ui {
class DockOptimizer;
}

class QTableWidgetItem;

class OpticalDevice;

class DockOptimizer : public QDockWidget
{
    Q_OBJECT

public:
    explicit DockOptimizer(QWidget *parent = 0);
    ~DockOptimizer();

    void device_changed(OpticalDevice *pDevice, int iReason);
    void save_to_device();

private slots:
    void on_pushButton_clicked();

private:
    void load_from_device();

    Ui::DockOptimizer *ui;
    OpticalDevice* _pDevice;
    DeviceOptimizer _dopt;
    int _iNbSurfaces;

};

#endif // DOCKOPTIMIZER_H
