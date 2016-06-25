#include "DockOptimizer.h"
#include "ui_DockOptimizer.h"

#include <sstream>

#include <QComboBox>
#include <QCheckBox>

#include "DeviceOptimizer.h"
#include "MainWindow.h"

#define NB_PARAM_MAX 6  // TODO use a dynamic ui list

/////////////////////////////////////////////////////////////

DockOptimizer::DockOptimizer(QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::DockOptimizer)
{
    ui->setupUi(this);
    _pDevice=0;
    _iNbSurfaces=0;

    QStringList qsl;
    qsl+="Optimize";
    qsl+="Surface";
    qsl+="Parameter";
    qsl+="Min";
    qsl+="Max";

    ui->twParams->clearContents();
    ui->twParams->setRowCount(NB_PARAM_MAX);

    ui->twParams->setColumnCount(qsl.size());
    ui->twParams->setHorizontalHeaderLabels (qsl);

    for(int i=0;i<NB_PARAM_MAX;i++)
    {
        QCheckBox* qcbOptimize=new QCheckBox;
        qcbOptimize->setChecked(false);
        ui->twParams->setCellWidget(i,0,qcbOptimize);

        QComboBox*  qcbParam=new QComboBox;
        qcbParam->addItem("");
        qcbParam->addItem("RCurv");
        qcbParam->addItem("Curvature");
        qcbParam->addItem("Conic");
        qcbParam->addItem("Thick");
        qcbParam->addItem("Z");
        qcbParam->addItem("r4");
        qcbParam->addItem("r6");
        qcbParam->addItem("r8");
        qcbParam->addItem("r10");

        ui->twParams->setCellWidget(i,2,qcbParam);

        QTableWidgetItem* pItemMin=new QTableWidgetItem;
        ui->twParams->setItem(i,3,pItemMin);

        QTableWidgetItem* pItemMax=new QTableWidgetItem;
        ui->twParams->setItem(i,4,pItemMax);
    }
    ui->twParams->resizeColumnsToContents();
}
/////////////////////////////////////////////////////////////
DockOptimizer::~DockOptimizer()
{
    delete ui;
}
/////////////////////////////////////////////////////////////
void DockOptimizer::on_pushButton_clicked()
{
    if(_pDevice==0)
        return;

    _dopt.clear();
    _dopt.set_device(_pDevice);

    for(int i=0;i<NB_PARAM_MAX;i++)
    {
        bool bOk=true;

        QCheckBox* qcbOptimize=(QCheckBox*)ui->twParams->cellWidget(i,0);
        if(!qcbOptimize->isChecked())
            continue;

        QComboBox* qcbSurf=(QComboBox*)ui->twParams->cellWidget(i,1);
        int iSurface=qcbSurf->currentText().toInt(&bOk)-1;
        if(!bOk)
            continue;

        QComboBox* qcbParam=(QComboBox*)ui->twParams->cellWidget(i,2);
        std::string sParam=qcbParam->currentText().toStdString();
        if(sParam=="")
            continue;

        QTableWidgetItem* pItemMin=ui->twParams->item(i,3);
        if(pItemMin==0)
            continue;
        double dMin=pItemMin->text().toDouble(&bOk);
        if(!bOk)
            continue;

        QTableWidgetItem* pItemMax=ui->twParams->item(i,4);
        if(pItemMax==0)
            continue;
        double dMax=pItemMax->text().toDouble(&bOk);
        if(!bOk)
            continue;

        _dopt.add_parameter(iSurface,sParam,dMin,dMax);
    }

    int iCriteria=ui->cbCriteria->currentIndex();
    OptimizerMeritFunction omf;
    if(iCriteria==0)
        omf=eCenterOnly;

    if(iCriteria==1)
        omf=eMostlyCenter;

    if(iCriteria==2)
        omf=eFullFrameMean;

    if(iCriteria==3)
        omf=eFullFrameMaxError;

    setCursor(Qt::WaitCursor);
    ui->lblResult->setText("Optimizing...");
    ui->lblResult->setStyleSheet("color: black;");
    ui->lblResult->repaint();

    _dopt.set_merit_function(omf);
    OptimizerResult result=_dopt.optimise(); //todo, put in thread
    if(result==eBetterSolutionFound)
    {
        ui->lblResult->setText("Better Solution found.");
        ui->lblResult->setStyleSheet("color: green;");
    }

    if(result==eNoBetterSolution)
    {
        ui->lblResult->setText("No Better Solution found.");
        ui->lblResult->setStyleSheet("color: red;");
    }

    if(result==eSolutionOnEdge)
    {
        ui->lblResult->setText("Solution on domain edge.");
        ui->lblResult->setStyleSheet("color: red;");
    }

    if(result==eNothingToOptimize)
    {
        ui->lblResult->setText("Nothing to optimize");
        ui->lblResult->setStyleSheet("color: black;");
    }

    setCursor(Qt::ArrowCursor);

    // update results
    if( (result==eBetterSolutionFound)|| (result==eSolutionOnEdge) )
        static_cast<MainWindow*>(parent())->update_views(this,OPTICAL_DEVICE_CHANGED);
}
/////////////////////////////////////////////////////////////
void DockOptimizer::device_changed(OpticalDevice *pDevice,int iReason)
{
    _pDevice=pDevice;
    int iNbSurfaces=_pDevice->nb_surface();

    if((iReason==NEW_OPTICAL_DEVICE) || (_iNbSurfaces!=iNbSurfaces) )
    {
        //reset all
        for(int i=0;i<NB_PARAM_MAX;i++)
        {
            QComboBox*  qcbSurf=new QComboBox;
            qcbSurf->addItem("");
            for(int i=0;i<iNbSurfaces;i++)
                qcbSurf->addItem(QString::number(i+1));

            ui->twParams->setCellWidget(i,1,qcbSurf);
        }
        _iNbSurfaces=iNbSurfaces;

        //init with parameters
        if(iReason==NEW_OPTICAL_DEVICE)
            load_from_device();
    }

    if( (iReason!=COMMENT_CHANGED) && (iReason!=OPTICAL_DEVICE_SAVED) )
    {
        ui->lblResult->setText("...");
        ui->lblResult->setStyleSheet("color: black;");
    }
}
/////////////////////////////////////////////////////////////
void DockOptimizer::save_to_device()
{
    if(_pDevice==0)
        return;

    for(int iRow=0;iRow<ui->twParams->rowCount();iRow++)
    {
        stringstream ss;
        ss << iRow;

        //todo write only if no default parameter
        //todo clean parameter if void

        QCheckBox* qcbOptimize=(QCheckBox*)ui->twParams->cellWidget(iRow,0);
        if(qcbOptimize)
            _pDevice->set_parameter("optimizer."+ss.str()+".checked",qcbOptimize->isChecked()?1:0);

        QComboBox* qcbSurf=(QComboBox*)ui->twParams->cellWidget(iRow,1);
        if(qcbSurf)
            _pDevice->set_parameter("optimizer."+ss.str()+".surface",qcbSurf->currentText().toStdString());

        QComboBox* qcbParam=(QComboBox*)ui->twParams->cellWidget(iRow,2);
        if(qcbParam)
            _pDevice->set_parameter("optimizer."+ss.str()+".parameter",qcbParam->currentText().toStdString());

        QTableWidgetItem* pItemMin=ui->twParams->item(iRow,3);
        if(pItemMin)
            _pDevice->set_parameter("optimizer."+ss.str()+".min",pItemMin->text().toStdString());

        QTableWidgetItem* pItemMax=ui->twParams->item(iRow,4);
        if(pItemMax)
            _pDevice->set_parameter("optimizer."+ss.str()+".max",pItemMax->text().toStdString());
    }
}
/////////////////////////////////////////////////////////////
void DockOptimizer::load_from_device()
{
    if(_pDevice==0)
        return;

    for(int iRow=0;iRow<ui->twParams->rowCount();iRow++)
    {
        stringstream ss;
        ss << iRow;

        QCheckBox* qcbOptimize=(QCheckBox*)ui->twParams->cellWidget(iRow,0);
        string sOptimize="0";
        _pDevice->get_parameter("optimizer."+ss.str()+".checked",sOptimize);
        qcbOptimize->setChecked(sOptimize!="0");

        QComboBox* qcbSurf=(QComboBox*)ui->twParams->cellWidget(iRow,1);
        string sSurface;
        _pDevice->get_parameter("optimizer."+ss.str()+".surface",sSurface);
        qcbSurf->setCurrentText(sSurface.c_str());

        QComboBox* qcbParam=(QComboBox*)ui->twParams->cellWidget(iRow,2);
        string sParam;
        _pDevice->get_parameter("optimizer."+ss.str()+".parameter",sParam);
        qcbParam->setCurrentText(sParam.c_str());

        QTableWidgetItem* pItemMin=ui->twParams->item(iRow,3);
        string sMin;
        _pDevice->get_parameter("optimizer."+ss.str()+".min",sMin);
        pItemMin->setText(sMin.c_str());

        QTableWidgetItem* pItemMax=ui->twParams->item(iRow,4);
        string sMax;
        _pDevice->get_parameter("optimizer."+ss.str()+".max",sMax);
        pItemMax->setText(sMax.c_str());
    }
}
/////////////////////////////////////////////////////////////
