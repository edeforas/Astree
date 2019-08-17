// this file is covered by the General Public License version 2 or later
// please see GPL.html for more details and licensing issues
// copyright Etienne de Foras ( the author )  mailto: etienne.deforas@gmail.com

#ifndef DOCKSCATTERPLOT_H
#define DOCKSCATTERPLOT_H

#include <QDockWidget>

namespace Ui
{
    class DockScatterPlot;
}

class OpticalDevice;

class DockScatterPlot : public QDockWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(DockScatterPlot)
public:
    explicit DockScatterPlot(QWidget *parent = 0);
    virtual ~DockScatterPlot();

    void device_changed(OpticalDevice *pDevice, int iReason);

protected:
    virtual void changeEvent(QEvent *e);

private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

    void on_hsImageFieldPos_valueChanged(int value);

private:
    Ui::DockScatterPlot *m_ui;
    OpticalDevice* _pDevice;
    bool _bBlockSignals;

    static void enlarge(QRectF& r,double dRatio);
};

#endif // DOCKSCATTERPLOT_H
