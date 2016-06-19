#include "DockOptimizer.h"
#include "ui_DockOptimizer.h"

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
        qcbParam->addItem("(none)");
        qcbParam->addItem("RCurv");
        qcbParam->addItem("Curvature");
        qcbParam->addItem("Conic");
        qcbParam->addItem("Thick");
        qcbParam->addItem("Z");
        qcbParam->addItem("r4");
        qcbParam->addItem("r6");
        qcbParam->addItem("r8");
        qcbParam->addItem("r10");

        //   qcbParam->setCurrentIndex(0);
        ui->twParams->setCellWidget(i,2,qcbParam);
        //connect(qcbType,SIGNAL(activated(int)),this&,SLOT(onTypeChanged()));
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
            return;

        QComboBox* qcbParam=(QComboBox*)ui->twParams->cellWidget(i,2);
        std::string sParam=qcbParam->currentText().toStdString();
        if(sParam=="(none)")
            continue;

        QTableWidgetItem* pItemMin=ui->twParams->item(i,3);
        if(pItemMin==0)
            continue;
        if(pItemMin->text().size()==0)
            continue;

        QTableWidgetItem* pItemMax=ui->twParams->item(i,4);
        if(pItemMax==0)
            continue;
        if(pItemMax->text().size()==0)
            continue;

        double dMin=pItemMin->text().toDouble(&bOk);
        if(!bOk)
            return;

        double dMax=pItemMax->text().toDouble(&bOk);
        if(!bOk)
            return;

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
        static_cast<MainWindow*>(parent())->update_views(this,PARAMETERS_CHANGED);
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
            for(int i=0;i<iNbSurfaces;i++)
                qcbSurf->addItem(QString::number(i+1));

            ui->twParams->setCellWidget(i,1,qcbSurf);
        }
        _iNbSurfaces=iNbSurfaces;
    }

    if( (iReason!=COMMENT_CHANGED) && (iReason!=OPTICAL_DEVICE_SAVED) )
    {
        ui->lblResult->setText("...");
        ui->lblResult->setStyleSheet("color: black;");
    }
}
/////////////////////////////////////////////////////////////
