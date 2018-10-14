#include "DockImageQuality.h"
#include "ui_DockImageQuality.h"

#include "OpticalDevice.h"
#include "AstreeDefines.h"
#include "MainWindow.h"

#include <cassert>

DockImageQuality::DockImageQuality(QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::DockImageQuality)
{
    _bBlockSignals=true;

    ui->setupUi(this);
    _pDevice=0;

    _bBlockSignals=false;
}
//////////////////////////////////////////////////////////////////////////////////
DockImageQuality::~DockImageQuality()
{
    delete ui;
}
//////////////////////////////////////////////////////////////////////////////////
void DockImageQuality::device_changed(OpticalDevice* pDevice,int iReason)
{
    assert(pDevice!=0);

    _pDevice=pDevice;

    if( (iReason!=OPTICAL_DEVICE_CHANGED) && (iReason!=NEW_OPTICAL_DEVICE) && ((iReason!=LIGHT_OFF_AXIS_CHANGED)))
        return;

    _bBlockSignals=true;

    ImageQuality pIQ=pDevice->get_image_quality();
    bool bInfinite=pIQ.isImageInfinite;
    int iNbAngles=pIQ.nb_angles();

    QStringList qsl;
    qsl+="Angle(deg)";

    if(bInfinite)
        qsl+="OutAngle(deg)";
    else
        qsl+="Dist(mm)";

    if(bInfinite)
        qsl+="SpotSize(deg)";
    else
        qsl+="SpotSize(µm)";


    if(bInfinite)
        qsl+="Spot/EyeLimit";
    else
        qsl+="Spot/Airy";

    qsl+="Vignetting";

    ui->tableWidget->clearContents();
    ui->tableWidget->setRowCount(iNbAngles);
    ui->tableWidget->setColumnCount(qsl.size());
    ui->tableWidget->setHorizontalHeaderLabels(qsl);

    for(int i=0;i<iNbAngles;i++)
    {
        double dAngle=pIQ.vdAngles[i];
        QString qsAngle=QString::number(dAngle,'g',3);
        ui->tableWidget->setItem(i,0,new QTableWidgetItem(qsAngle));

        double dDist=pIQ.vdDist[i];
        QString qsDist=QString::number(dDist,'g',3);
        ui->tableWidget->setItem(i,1,new QTableWidgetItem(qsDist));

        double dSpotSize=pIQ.vdSpotSize[i];
        if(!bInfinite)
            dSpotSize*=1000.; //convert to microns

        QString qsSpotSize=QString::number(dSpotSize,'g',3);
        ui->tableWidget->setItem(i,2,new QTableWidgetItem(qsSpotSize));

        double dSpotvsAiry=pIQ.vdSpotvsAiry[i];
        QString qsSpotvsAiry=QString::number(dSpotvsAiry,'g',3);
        ui->tableWidget->setItem(i,3,new QTableWidgetItem(qsSpotvsAiry));

        double dVignetting=pIQ.vdVignetting[i];
        QString qsVignetting=QString::number(dVignetting,'f',1)+QString(" %");
        ui->tableWidget->setItem(i,4,new QTableWidgetItem(qsVignetting));


        //....

    }

    //  ui->tableWidget->setAlternatingRowColors(true);
    ui->tableWidget->resizeColumnsToContents();

    if( iNbAngles!=0 )
    {
        if(bInfinite)
        {
            ui->lAirySize->setText(QString::number(pIQ.dAirySize,'g',3)+QString(" deg"));
            ui->lFNumber->setText("n/a");
        }
        else
        {
            ui->lAirySize->setText(QString::number(pIQ.dAirySize*1000.,'g',3)+QString(" µm"));
            ui->lFNumber->setText(QString::number(pIQ.dFNumber,'g',3));
        }
    }
    else
    {
        ui->lAirySize->setText("n/a");
        ui->lFNumber->setText("n/a");
    }

    //update angle selected row
    double dPercentField;
    if( pDevice->get_parameter("showLightOffAxis",dPercentField) && (ui->tableWidget->rowCount()>1))
    {
        ui->tableWidget->selectRow((int)(0.5+(ui->tableWidget->rowCount()-1)*dPercentField/100.));
    }
    _bBlockSignals=false;
}
//////////////////////////////////////////////////////////////////////////////////
void DockImageQuality::on_tableWidget_currentCellChanged(int currentRow, int currentColumn, int previousRow, int previousColumn)
{
    (void)currentColumn;
    (void)previousRow;
    (void)previousColumn;

    if(_bBlockSignals)
        return;

    assert(_pDevice);

    if(currentRow<0)
        return;

    if(ui->tableWidget->rowCount()>1.)
    {
        double dPercentField=(100.*currentRow)/(ui->tableWidget->rowCount()-1);

        assert(dPercentField>=0);
        assert(dPercentField<=100);

        _pDevice->set_parameter("showLightOffAxis",dPercentField);
        static_cast<MainWindow*>(parent())->device_changed(this,LIGHT_OFF_AXIS_CHANGED,false);
    }
}
//////////////////////////////////////////////////////////////////////////////////
