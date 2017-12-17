#include "DockOptimizer.h"
#include "ui_DockOptimizer.h"

#include <sstream>
#include <cassert>

#include <QComboBox>
#include <QCheckBox>

#include "DeviceOptimizerRandom.h"
#include "DeviceOptimizerAmoeba.h"

#include "MainWindow.h"

#define NB_PARAM_MAX 10  // TODO use a dynamic ui list

/////////////////////////////////////////////////////////////

DockOptimizer::DockOptimizer(QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::DockOptimizer)
{
    _bBlockSignals=true;

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
        connect(qcbOptimize,&QCheckBox::stateChanged,this,&DockOptimizer::tableChanged);

        QComboBox*  qcbSurf=new QComboBox;
        ui->twParams->setCellWidget(i,1,qcbSurf);
        connect(qcbSurf,&QComboBox::currentTextChanged,this,&DockOptimizer::tableChanged);

        QComboBox*  qcbParam=new QComboBox;
        qcbParam->addItem("");
        qcbParam->addItem("RCurv");
        //    qcbParam->addItem("Curvature");
        qcbParam->addItem("Conic");
        qcbParam->addItem("Thick");
        qcbParam->addItem("Z");
        qcbParam->addItem("R4");
        qcbParam->addItem("R6");
        qcbParam->addItem("R8");
        qcbParam->addItem("R10");
        ui->twParams->setCellWidget(i,2,qcbParam);
        connect(qcbParam,&QComboBox::currentTextChanged,this,&DockOptimizer::tableChanged);

        QTableWidgetItem* pItemMin=new QTableWidgetItem;
        ui->twParams->setItem(i,3,pItemMin);

        QTableWidgetItem* pItemMax=new QTableWidgetItem;
        ui->twParams->setItem(i,4,pItemMax);
    }
    ui->twParams->resizeColumnsToContents();
    _bBlockSignals=false;
}
/////////////////////////////////////////////////////////////
DockOptimizer::~DockOptimizer()
{
    delete ui;
}
/////////////////////////////////////////////////////////////
void DockOptimizer::on_pushButton_clicked()
{
    optimize(false);
}
/////////////////////////////////////////////////////////////
void DockOptimizer::optimize(bool bModeRefine)
{
    if(_pDevice==0)
        return;

    if(_pDevice->nb_surface()==0)
        return;

    DeviceOptimizer* optim;
    if(ui->cbMethod->currentIndex()==0)
        optim=new DeviceOptimizerAmoeba;
    else
        optim=new DeviceOptimizerRandom;

    optim->set_device(_pDevice);

    for(int i=0;i<NB_PARAM_MAX;i++)
    {
        bool bOk=true;
        bool bDefaultParam=false;

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
            bDefaultParam=true;

        QTableWidgetItem* pItemMax=ui->twParams->item(i,4);
        if(pItemMax==0)
            continue;
        double dMax=pItemMax->text().toDouble(&bOk);
        if(!bOk)
            bDefaultParam=true;

        //TODO use device properties instead of reading from the UI

        if(bDefaultParam)
            optim->add_parameter(iSurface,sParam,bModeRefine);
        else
            optim->add_parameter(iSurface,sParam,dMin,dMax);
    }

    int iCriteria=ui->cbCriteria->currentIndex();
    OptimizerMeritFunction omf=eCenterOnly;

    if(iCriteria==0)
        omf=eCenterOnly;

    if(iCriteria==1)
        omf=eMostlyCenter;

    if(iCriteria==2)
        omf=eFullFrameMean;

    if(iCriteria==3)
        omf=eFullFrameMaxError;

    optim->set_merit_function(omf);

    optim->set_min_vignetting(ui->leMinVignetting->text().toDouble());

    //now optimize
    setCursor(Qt::WaitCursor);
    ui->lblResult->setText("Optimizing...");
    ui->lblResult->setStyleSheet("color: black;");
    ui->lblResult->repaint();
    OptimizerResult result=optim->optimize(); //TODO, put in thread

    if(result==eBetterSolutionFound)
    {
        ui->lblResult->setText("Better Solution found.");
        ui->lblResult->setStyleSheet("color: green;");
    }

    if(result==eNoBetterSolution)
    {
        ui->lblResult->setText("No Better Solution found.");
        ui->lblResult->setStyleSheet("color: black;");
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
        static_cast<MainWindow*>(parent())->device_changed(this,OPTICAL_DEVICE_CHANGED);

    delete optim;
}
/////////////////////////////////////////////////////////////
void DockOptimizer::device_changed(OpticalDevice *pDevice,int iReason)
{
    _pDevice=pDevice;

    _bBlockSignals=true;

    int iNbSurfaces=_pDevice->nb_surface();

    if((iReason==NEW_OPTICAL_DEVICE) || (_iNbSurfaces!=iNbSurfaces) )
    {
        //reset all
        for(int i=0;i<NB_PARAM_MAX;i++)
        {
            QComboBox* qcbSurf=(QComboBox*)ui->twParams->cellWidget(i,1);
            qcbSurf->clear(); //TODO
            qcbSurf->addItem("");
            for(int i=0;i<iNbSurfaces;i++)
                qcbSurf->addItem(QString::number(i+1));
        }
        _iNbSurfaces=iNbSurfaces;

        //init with parameters
        if(iReason==NEW_OPTICAL_DEVICE)
        {
            for(int iRow=0;iRow<ui->twParams->rowCount();iRow++)
            {
                stringstream ss;
                ss << iRow;

                QCheckBox* qcbOptimize=(QCheckBox*)ui->twParams->cellWidget(iRow,0);
                string sOptimize;
                _pDevice->get_parameter("optimizer."+ss.str()+".checked",sOptimize);
                qcbOptimize->setChecked(sOptimize=="1");

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
                if(_pDevice->get_parameter("optimizer."+ss.str()+".min",sMin))
                    pItemMin->setText(sMin.c_str());
                else
                    pItemMin->setText("");

                QTableWidgetItem* pItemMax=ui->twParams->item(iRow,4);
                string sMax;
                if(_pDevice->get_parameter("optimizer."+ss.str()+".max",sMax))
                    pItemMax->setText(sMax.c_str());
                else
                    pItemMax->setText("");
            }

            //load merit function
            string sMeritFunction="CenterOnly";
            _pDevice->get_parameter("optimizer.merit",sMeritFunction);

            if(sMeritFunction=="CenterOnly")
                ui->cbCriteria->setCurrentIndex(0);

            if(sMeritFunction=="MostlyCenter")
                ui->cbCriteria->setCurrentIndex(1);

            if(sMeritFunction=="FullFrameMean")
                ui->cbCriteria->setCurrentIndex(2);

            if(sMeritFunction=="FullFrameMaxError")
                ui->cbCriteria->setCurrentIndex(3);

            //load optim method
            string sOptimFunction="Unknow";
            _pDevice->get_parameter("optimizer.method",sOptimFunction);

            if(sOptimFunction=="NelderMead")
                ui->cbMethod->setCurrentIndex(0);

            if(sOptimFunction=="MonteCarlo")
                ui->cbMethod->setCurrentIndex(1);

            // vignetting setting
            string sMinVignetting="100.";
            _pDevice->get_parameter("optimizer.minVignetting",sMinVignetting);
            ui->leMinVignetting->setText(sMinVignetting.c_str());
        }
    }

    if( (iReason!=COMMENT_CHANGED) && (iReason!=OPTICAL_DEVICE_SAVED) )
    {
        ui->lblResult->setText("...");
        ui->lblResult->setStyleSheet("color: black;");
    }

    _bBlockSignals=false;
}
/////////////////////////////////////////////////////////////
void DockOptimizer::on_twParams_cellChanged(int row, int column)
{
    (void)column;
    (void)row;

    tableChanged();
}
//////////////////////////////////////////////////////////
void DockOptimizer::on_cbCriteria_currentTextChanged(const QString &arg1)
{
    (void)arg1;

    if(_bBlockSignals)
        return;

    //save merit function
    int iCriteria=ui->cbCriteria->currentIndex();

    if(iCriteria==0)
        _pDevice->set_parameter("optimizer.merit","CenterOnly");

    if(iCriteria==1)
        _pDevice->set_parameter("optimizer.merit","MostlyCenter");

    if(iCriteria==2)
        _pDevice->set_parameter("optimizer.merit","FullFrameMean");

    if(iCriteria==3)
        _pDevice->set_parameter("optimizer.merit","FullFrameMaxError");

    static_cast<MainWindow*>(parent())->device_changed(this,OPTIMIZER_CHANGED);
}
/////////////////////////////////////////////////////////////
void DockOptimizer::tableChanged()
{
    if(_bBlockSignals)
        return;

    for(int row=0;row<ui->twParams->rowCount();row++)
    {
        stringstream ss;
        ss << row;

        //TODO write only if no default parameter
        //TODO clean parameter if void FIXME

        QCheckBox* qcbOptimize=(QCheckBox*)ui->twParams->cellWidget(row,0);
        assert(qcbOptimize);
        _pDevice->set_parameter("optimizer."+ss.str()+".checked",qcbOptimize->isChecked()?1:0);

        QComboBox* qcbSurf=(QComboBox*)ui->twParams->cellWidget(row,1);
        assert(qcbSurf);
        _pDevice->set_parameter("optimizer."+ss.str()+".surface",qcbSurf->currentText().toStdString());

        QComboBox* qcbParam=(QComboBox*)ui->twParams->cellWidget(row,2);
        assert(qcbParam);
        _pDevice->set_parameter("optimizer."+ss.str()+".parameter",qcbParam->currentText().toStdString());

        QTableWidgetItem* pItemMin=ui->twParams->item(row,3);
        assert(pItemMin);
        _pDevice->set_parameter("optimizer."+ss.str()+".min",pItemMin->text().toStdString());

        QTableWidgetItem* pItemMax=ui->twParams->item(row,4);
        assert(pItemMax);
        _pDevice->set_parameter("optimizer."+ss.str()+".max",pItemMax->text().toStdString());
    }

    static_cast<MainWindow*>(parent())->device_changed(this,OPTIMIZER_CHANGED);
}
/////////////////////////////////////////////////////////////
void DockOptimizer::on_cbMethod_currentIndexChanged(int index)
{
    (void)index;

    if(_bBlockSignals)
        return;

    //save merit function
    int iCriteria=ui->cbMethod->currentIndex();

    if(iCriteria==0)
        _pDevice->set_parameter("optimizer.method","NelderMead");

    if(iCriteria==1)
        _pDevice->set_parameter("optimizer.method","MonteCarlo");

    static_cast<MainWindow*>(parent())->device_changed(this,OPTIMIZER_CHANGED);
}
//////////////////////////////////////////////////////////
void DockOptimizer::on_pushButton_2_clicked()
{
    optimize(true);
}
//////////////////////////////////////////////////////////
void DockOptimizer::on_leMinVignetting_editingFinished()
{
    if(_bBlockSignals)
        return;

    //save merit function
    double dMinVignetting=ui->leMinVignetting->text().toDouble();
    if( (dMinVignetting>100.)|| (dMinVignetting<=0.))
        dMinVignetting=100.;

    _pDevice->set_parameter("optimizer.minVignetting",dMinVignetting);

    static_cast<MainWindow*>(parent())->device_changed(this,OPTIMIZER_CHANGED);
}
//////////////////////////////////////////////////////////
