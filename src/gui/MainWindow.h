// this file is covered by the General Public License version 2 or later
// please see GPL.html for more details and licensing issues
// copyright Etienne de Foras ( the author )  mailto: etienne.deforas@gmail.com

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "AstreeDefines.h"

#include <string>
#include <vector>
using namespace std;

namespace Ui
{
class MainWindowClass;
}

class OpticalDevice;
class DockSurfacesData;
class FrameSideView;
class DockLightProperties;
class DockScatterPlot;
class DockAutofocus;
class DockImageQuality;
class DockCommentary;
class DockOptimizer;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

    bool load_file(string sFile);
    bool ask_save_and_action();
    void clear_device();

public slots:
    void device_changed(void* pSender, int iReason, bool bMustSave=true);

protected:
    virtual void closeEvent ( QCloseEvent * event );
    virtual void resizeEvent( QResizeEvent *e );

private:
    void glass_dialog(bool bSelect);
    Ui::MainWindowClass *ui;

private slots:
    void on_actionNew_triggered();
    void on_actionLoad_triggered();
    void on_actionSave_triggered();
    void on_actionSave_as_triggered();
    void on_actionClose_triggered();
    void on_actionAbout_triggered();
    void on_actionQuit_triggered();
    void on_actionMedium_Manager_triggered();
    void on_actionScal_device_triggered();
    void on_actionImport_ZMX_file_triggered();
    void on_actionFit_in_View_triggered();
    void on_actionZoom_in_triggered();
    void on_actionZoom_out_triggered();

    void on_actionReload_triggered();

private:
    OpticalDevice* _pDevice;
    DockSurfacesData* _pDockSurfacesData;
    DockLightProperties* _pDockLightProperties;
    DockAutofocus* _pDockAutofocus;
    DockScatterPlot* _pDockScatterPlot;
    DockCommentary* _pDockCommentary;
    DockOptimizer* _pDockOptimizer;
    FrameSideView* _pFrameSideView;
    DockImageQuality* _pDockImageQuality;
    bool _bMustSave;
    string _sFileName;
};

#endif // MAINWINDOW_H
