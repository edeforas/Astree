// this file is covered by the General Public License version 2 or later
// please see GPL.html for more details and licensing issues
// copyright Etienne de Foras ( the author )  mailto: etienne.deforas@gmail.com

#include "MainWindow.h"
#include "ui_MainWindow.h"
#include <QMessageBox>
#include <QFileDialog>
#include <QtGui>

#include <cassert>

#include "OpticalDevice.h"
#include "DeviceIo.h"
#include "DeviceIoZmx.h"
#include "DockSurfacesData.h"
#include "DockLightProperties.h"
#include "DockScatterPlot.h"
#include "DockCommentary.h"
#include "DockImageQuality.h"
#include "DockOptimizer.h"
#include "FrameSideView.h"
#include "AstreeDefines.h"
#include "FileUtil.h"
#include "GlassManager.h"
#include "DialogMediumManager.h"
#include "DialogScaleDevice.h"

#include "GlassCatalogIo.h"

#include "DeviceScaling.h"
//////////////////////////////////////////////////////////////////////////////
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindowClass)
{
    ui->setupUi(this);
    _pDevice=new OpticalDevice;

    //load all glass catalog
    string sExepath=FileUtil::get_path(FileUtil::get_executable_path());
    vector<string> vsCatalog=FileUtil::list(sExepath+"\\glass\\*.agf");
    for(unsigned int i=0;i<vsCatalog.size();i++)
    {
        bool bOk=GlassCatalogIO::load(sExepath+"\\glass\\"+vsCatalog[i],GlassManager::singleton());
        if(bOk==false)
            QMessageBox::warning(this,"Warning:","Unable to load glass catalog: "+QString(vsCatalog[i].c_str()));
    }

    _bMustSave=false;

    _pFrameSideView=new FrameSideView(this);
    setCentralWidget(_pFrameSideView);

    _pDockSurfacesData=new DockSurfacesData(this);
    addDockWidget(Qt::BottomDockWidgetArea, _pDockSurfacesData);

    _pDockLightProperties=new DockLightProperties(this);
    addDockWidget(Qt::LeftDockWidgetArea, _pDockLightProperties);

    _pDockScatterPlot=new DockScatterPlot(this);
    addDockWidget(Qt::LeftDockWidgetArea, _pDockScatterPlot);

    _pDockCommentary=new DockCommentary(this);
    addDockWidget(Qt::LeftDockWidgetArea, _pDockCommentary);

    _pDockImageQuality=new DockImageQuality(this);
    addDockWidget(Qt::BottomDockWidgetArea, _pDockImageQuality);

    _pDockOptimizer=new DockOptimizer(this);
    addDockWidget(Qt::LeftDockWidgetArea, _pDockOptimizer);


    tabifyDockWidget(_pDockCommentary,_pDockOptimizer);
    tabifyDockWidget(_pDockOptimizer,_pDockLightProperties);
    tabifyDockWidget(_pDockLightProperties,_pDockScatterPlot);
    //  tabifyDockWidget(_pDockScatterPlot,_pDockImageQuality);

    device_changed(0,NEW_OPTICAL_DEVICE,false);
}
//////////////////////////////////////////////////////////////////////////////
MainWindow::~MainWindow()
{
    delete ui;
    delete _pDevice;
}
//////////////////////////////////////////////////////////////////////////////
void MainWindow::closeEvent(QCloseEvent *event)
{
    if (ask_save_and_action())
        event->accept();
    else
        event->ignore();
}
//////////////////////////////////////////////////////////////////////////////
bool MainWindow::ask_save_and_action()
{
    if (_bMustSave==true)
    {
        int iRet=QMessageBox::question(this,"Warning","Save?",QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
        if (iRet==QMessageBox::Cancel)
            return false;

        if (iRet==QMessageBox::Yes)
        {
            DeviceIo::save(_sFileName,_pDevice);
            _bMustSave=false;
            device_changed(0,OPTICAL_DEVICE_SAVED);
        }
    }

    return true;
}
//////////////////////////////////////////////////////////////////////////////
void MainWindow::clear_device()
{
    _bMustSave=false;
    delete _pDevice;
    _pDevice=new OpticalDevice;
}
//////////////////////////////////////////////////////////////////////////////
void MainWindow::on_actionQuit_triggered()
{
    if (ask_save_and_action())
        qApp->quit();
}
//////////////////////////////////////////////////////////////////////////////
void MainWindow::on_actionAbout_triggered()
{
    QMessageBox mb;
    mb.setText("Astree " ASTREE_VERSION " by etienne.deforas@gmail.com");
    mb.setIconPixmap(QPixmap(":/Astree/Astree.ico"));
    mb.exec();
}
//////////////////////////////////////////////////////////////////////////////
void MainWindow::on_actionLoad_triggered()
{
    if (ask_save_and_action()==false)
        return;

    _sFileName = QFileDialog::getOpenFileName(this,tr("Open Astree File"), ".", tr("Astree Files (*.astree)")).toStdString();

    if (!_sFileName.empty())
    {
        clear_device();
        device_changed(0,NEW_OPTICAL_DEVICE,false);
        load_file(_sFileName);
    }
}
//////////////////////////////////////////////////////////////////////////////
void MainWindow::on_actionNew_triggered()
{
    if (ask_save_and_action()==false)
        return;

    clear_device();
    device_changed(0,NEW_OPTICAL_DEVICE,false);
    _pFrameSideView->fit_in_view();
}
//////////////////////////////////////////////////////////////////////////////
void MainWindow::on_actionSave_triggered()
{
    if(_sFileName.empty())
        on_actionSave_as_triggered();
    else
    {
        DeviceIo::save(_sFileName,_pDevice);
        _bMustSave=false;
        device_changed(0,OPTICAL_DEVICE_SAVED,false);
    }
}
//////////////////////////////////////////////////////////////////////////////
void MainWindow::on_actionSave_as_triggered()
{
    if (_pDevice==0)
        return;

    string sFileName = QFileDialog::getSaveFileName(this,tr("Open Astree File"), ".", tr("Astree Files (*.astree)")).toStdString();

    if (sFileName.empty()==false)
    {
        _sFileName=sFileName;
        DeviceIo::save(_sFileName,_pDevice);
        _bMustSave=false;
        device_changed(0,OPTICAL_DEVICE_SAVED,false);
    }
}
//////////////////////////////////////////////////////////////////////////////
void MainWindow::on_actionClose_triggered()
{
    if (ask_save_and_action()==false)
        return;

    clear_device();
    _sFileName="";
    device_changed(0,NEW_OPTICAL_DEVICE,false);
    _pFrameSideView->fit_in_view();
}
//////////////////////////////////////////////////////////////////////////////
bool MainWindow::load_file(string sFile)
{
    _sFileName="";
    _bMustSave=false;
    clear_device();
    OpticalDevice* pDevice=DeviceIo::load(sFile);
    if (pDevice==0)
    {
        QMessageBox m;
        m.setText((string("Unable to Open file!!:")+sFile).c_str());
        m.exec();
        return false;
    }

    delete _pDevice;
    _pDevice=pDevice;
    _sFileName=sFile;
    device_changed(0,NEW_OPTICAL_DEVICE,false);
    _pFrameSideView->fit_in_view();
    return true;
}
//////////////////////////////////////////////////////////////////////////////
void MainWindow::device_changed(void* pSender,int iReason,bool bMustSave)
{
    if(bMustSave)
        _bMustSave=true;

    if (pSender!=_pDockScatterPlot)
    {
        _pDockScatterPlot->device_changed(_pDevice,iReason);
    }

    if (pSender!=_pDockSurfacesData)
    {
        _pDockSurfacesData->device_changed(_pDevice,iReason);
    }

    if (pSender!=_pDockLightProperties)
    {
        _pDockLightProperties->device_changed(_pDevice,iReason);
    }

    if (pSender!=_pFrameSideView)
    {
        _pFrameSideView->device_changed(_pDevice,iReason);
    }

    if(pSender!=_pDockCommentary)
    {
        _pDockCommentary->device_changed(_pDevice,iReason);
    }

    if(pSender!=_pDockImageQuality)
    {
        _pDockImageQuality->device_changed(_pDevice,iReason);
    }

    if(pSender!=_pDockOptimizer)
    {
        _pDockOptimizer->device_changed(_pDevice,iReason);
    }

    if(iReason==CHANGE_GLASS)
        glass_dialog(true);

    //update title
    string sTitle="Astree";
    if(!_sFileName.empty())
        sTitle+=" ["+_sFileName+"]";

    if (_bMustSave==true)
        sTitle+=" *";

    setWindowTitle(QString(sTitle.c_str()));
}
//////////////////////////////////////////////////////////////////////////////
void MainWindow::glass_dialog(bool bSelect)
{
    int iSurf=_pDockSurfacesData->selected_surface();
    string sGlass;

    if(bSelect)
        sGlass=_pDevice->type(iSurf);

    DialogMediumManager mm(this,sGlass);
    if(mm.exec()!=QDialog::Accepted)
        return;

    string sSelectedGlass=mm.selected_glass();

    if( (!sSelectedGlass.empty()) && bSelect)
    {
        _pDevice->set_type(iSurf,sSelectedGlass);
        device_changed(0,OPTICAL_DEVICE_CHANGED);
    }
}
//////////////////////////////////////////////////////////////////////////////
void MainWindow::on_actionMedium_Manager_triggered()
{
    glass_dialog(false);
}
//////////////////////////////////////////////////////////////////////////////
void MainWindow::on_actionScal_device_triggered()
{
    DialogScaleDevice sd(this);
    if(sd.exec())
    {
        DeviceScaling dmc;

        dmc.scale(_pDevice,sd.get_scale(),sd.get_scale_diameter(),sd.get_scale_focal());
        _bMustSave=true;
        device_changed(0,OPTICAL_DEVICE_CHANGED);
        _pFrameSideView->fit_in_view();
    }
}
//////////////////////////////////////////////////////////////////////////////
void MainWindow::on_actionImport_ZMX_file_triggered()
{
    if (ask_save_and_action()==false)
        return;

    string sFile = QFileDialog::getOpenFileName(this,tr("Import ZMX File"), ".", tr("ZMX Files (*.zmx)")).toStdString();

    if (!sFile.empty())
    {
        _sFileName="";
        clear_device();
        device_changed(0,NEW_OPTICAL_DEVICE);

        _bMustSave=false;
        clear_device();
        OpticalDevice* pDevice=DeviceIoZmx::import(sFile);
        if (pDevice==0)
        {
            QMessageBox m;
            m.setText((string("Unable to Open Zmx file!!:")+_sFileName).c_str());
            m.exec();
            return;
        }

        delete _pDevice;
        _pDevice=pDevice;
        _sFileName=sFile;
        device_changed(0,NEW_OPTICAL_DEVICE);
        _pFrameSideView->fit_in_view();
    }
}
//////////////////////////////////////////////////////////////////////////////
void MainWindow::on_actionFit_in_View_triggered()
{
    _pFrameSideView->fit_in_view();
}
//////////////////////////////////////////////////////////////////////////////
void MainWindow::on_actionZoom_in_triggered()
{
    _pFrameSideView->zoom_in();
}
//////////////////////////////////////////////////////////////////////////////
void MainWindow::on_actionZoom_out_triggered()
{
    _pFrameSideView->zoom_out();
}
//////////////////////////////////////////////////////////////////////////////
void MainWindow::on_actionReload_triggered()
{
    if (ask_save_and_action()==false)
        return;

    clear_device();
    device_changed(0,NEW_OPTICAL_DEVICE,false);
    if (!_sFileName.empty())
        load_file(_sFileName);
}
//////////////////////////////////////////////////////////////////////////////
void MainWindow::resizeEvent( QResizeEvent *e )
{
    (void)e;
    //  MainWindow::resizeEvent(e);

    _pDockScatterPlot->device_changed(_pDevice,OPTICAL_DEVICE_CHANGED);
    _pFrameSideView->fit_in_view();
}
