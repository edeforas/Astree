// this file is covered by the General Public License version 2 or later
// please see GPL.html for more details and licensing issues
// copyright Etienne de Foras ( the author )  mailto: etienne.deforas@gmail.com

#ifndef DOCKSURFACESDATA_H
#define DOCKSURFACESDATA_H

#include <QDockWidget>

#include <string>
using namespace std;

namespace Ui
{
class DockSurfacesData;
}

class OpticalDevice;

class DockSurfacesData : public QDockWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(DockSurfacesData)

public:

    explicit DockSurfacesData(QWidget *parent = 0);
    virtual ~DockSurfacesData();

    void device_changed(OpticalDevice* pDevice, int iReason);
    int selected_surface() const;

protected:
    virtual void changeEvent(QEvent *e);

private slots:
    void onTypeChanged();
    void on_cbPolyAspheric_clicked();
    void on_cbInnerDiameter_clicked();
    void on_cbComment_clicked();
    void on_comboCoordMode_activated(const QString &arg1);

    void on_btnAddSurfaceBefore_clicked();
    void on_btnAddSurfaceAfter_clicked();
    void on_btnDeleteSurface_clicked();

    void on_twSurfacesDatas_cellChanged(int row, int column);

private:
    void update_labels();
    void update_table();

    Ui::DockSurfacesData* m_ui;
    OpticalDevice* _pOD;
    bool _bBlockSignals;

    bool _bHaveAspheric;
    bool _bDisplayAspheric;

    bool _bHaveInnerDiameter;
    bool _bDisplayInnerDiameter;

    bool _bHaveComment;
    bool _bDisplayComment;
};

#endif // DOCKSURFACESDATA_H
