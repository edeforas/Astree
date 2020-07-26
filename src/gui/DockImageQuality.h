// this file is covered by the  GNU LESSER GENERAL PUBLIC LICENSE Version 3 or later
// please see LICENSE.txt for more details and licensing issues
// copyright Etienne de Foras ( the author )  mailto: etienne.deforas@gmail.com

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

     void on_tableWidget_itemSelectionChanged();

private:
    Ui::DockImageQuality *ui;
    OpticalDevice* _pDevice;

    void highlight_offaxislight_row();

    bool _bBlockSignals;
};

#endif // DOCKIMAGEQUALITY_H
