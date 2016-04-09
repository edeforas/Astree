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

     void device_changed(OpticalDevice *pDevice);

private:
    Ui::DockImageQuality *ui;
};

#endif // DOCKIMAGEQUALITY_H
