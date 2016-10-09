#include "DockImageQuality.h"
#include "ui_DockImageQuality.h"

#include "OpticalDevice.h"
#include "AstreeDefines.h"

#include <cassert>

DockImageQuality::DockImageQuality(QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::DockImageQuality)
{
    ui->setupUi(this);
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

    if( (iReason!=OPTICAL_DEVICE_CHANGED) && (iReason!=NEW_OPTICAL_DEVICE) )
        return;

    ImageQuality pIQ=pDevice->get_image_quality();
    int iNbAngles=pIQ.nb_angles();

    QStringList qsl;
    qsl+="Angle(deg)";
    qsl+="Dist(mm)";
    qsl+="SpotSize(µm)";
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

        double dSpotSize=pIQ.vdSpotSize[i]*1000.; //convert in microns
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

    if(iNbAngles!=0)
    {
        ui->lFNumber->setText(QString::number(pIQ.dFNumber,'g',3));
        ui->lAirySize->setText(QString::number(pIQ.dAirySize*1000.,'g',3)+QString(" µm"));
    }
    else
    {
        ui->lFNumber->setText("n/a");
        ui->lAirySize->setText("n/a");
    }
}
//////////////////////////////////////////////////////////////////////////////////
