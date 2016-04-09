// this file is covered by the General Public License version 2 or later
// please see GPL.html for more details and licensing issues
// copyright Etienne de Foras ( the author )  mailto: etienne.deforas@gmail.com

#ifndef DOCKSURFACESDATA_H
#define DOCKSURFACESDATA_H

#include <QDockWidget>

namespace Ui
{
    class DockSurfacesData;
}

class MainWindow;
class OpticalDevice;

class DockSurfacesData : public QDockWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(DockSurfacesData)

public:

    explicit DockSurfacesData(QWidget *parent = 0);
    virtual ~DockSurfacesData();

    void device_changed(OpticalDevice* pDevice);


public slots:
    void OnCellChanged(int iRow,int iCol);
    void OnAddSurfaceAfter();
    void OnAddSurfaceBefore();
    void OnDeleteSurface();
    void onTypeChanged();

protected:
    virtual void changeEvent(QEvent *e);

private slots:
    void on_cbPolyAspheric_clicked();
    void on_cbInnerDiameter_clicked();
    void on_cbComment_clicked();
    void on_comboCoordMode_activated(const QString &arg1);

private:
    void update_labels(OpticalDevice *pDevice);
    void update_table();

    Ui::DockSurfacesData* m_ui;
    OpticalDevice* _pDevice;
    bool _bCanEmit;

    bool _bHaveAspheric;
    bool _bDisplayAspheric;

    bool _bHaveInnerDiameter;
    bool _bDisplayInnerDiameter;

    bool _bHaveComment;
    bool _bDisplayComment;
};

#endif // DOCKSURFACESDATA_H
