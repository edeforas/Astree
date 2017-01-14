#ifndef DOCKIMAGEQUALITY_H
#define DOCKIMAGEQUALITY_H

#include <QDockWidget>

class OpticalDevice;

namespace Ui {
class DockImageQuality;
}

class DockImageQuality : public QDockWidget
{
    Q_OBJECT

public:
    explicit DockImageQuality(QWidget *parent = 0);
    ~DockImageQuality();

     void device_changed(OpticalDevice *pDevice, int iReason);

private slots:
     void on_tableWidget_currentCellChanged(int currentRow, int currentColumn, int previousRow, int previousColumn);

private:
    Ui::DockImageQuality *ui;
    OpticalDevice* _pDevice;

    bool _bBlockSignals;
};

#endif // DOCKIMAGEQUALITY_H
