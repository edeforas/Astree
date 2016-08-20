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

private slots:
    void on_pushButton_clicked();

    void on_twParams_cellChanged(int row, int column);

    void on_cbCriteria_currentTextChanged(const QString &arg1);

private:

    Ui::DockOptimizer *ui;
    OpticalDevice* _pDevice;
    DeviceOptimizer _dopt;
    int _iNbSurfaces;
    bool _bBlockSignals;

};

#endif // DOCKOPTIMIZER_H
