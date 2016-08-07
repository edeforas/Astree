// this file is covered by the General Public License version 2 or later
// please see GPL.html for more details and licensing issues
// copyright Etienne de Foras ( the author )  mailto: etienne.deforas@gmail.com

#include "DockSurfacesData.h"
#include "ui_DockSurfacesData.h"

#include <QComboBox>
#include <cassert>
#include <sstream>
#include "OpticalDevice.h"
#include "GlassManager.h"
#include "MainWindow.h"
#include "AstreeDefines.h"

#define ITEM_TYPE 0

DockSurfacesData::DockSurfacesData(QWidget *parent) :
    QDockWidget(parent),
    m_ui(new Ui::DockSurfacesData)
{
    pOD=0;
    m_ui->setupUi(this);

    m_ui->twSurfacesDatas->setColumnCount(8);
    update_labels(0);

    connect(m_ui->twSurfacesDatas,SIGNAL(cellChanged(int,int)),this,SLOT(OnCellChanged(int,int)));
    connect(m_ui->btnAddSurfaceAfter,SIGNAL(clicked()),this,SLOT(OnAddSurfaceAfter()));
    connect(m_ui->btnAddSurfaceBefore,SIGNAL(clicked()),this,SLOT(OnAddSurfaceBefore()));
    connect(m_ui->btnDeleteSurface,SIGNAL(clicked()),this,SLOT(OnDeleteSurface()));

    _bCanEmit=false;

    _bHaveAspheric=false;
    _bDisplayAspheric=false;

    _bHaveInnerDiameter=false;
    _bDisplayInnerDiameter=false;

    _bHaveComment=false;
    _bDisplayComment=false;
}
/////////////////////////////////////////////////////////////////////////://///
void DockSurfacesData::update_labels(OpticalDevice *pDevice)
{
    QStringList qsl;
    qsl+="Type";
    qsl+="RCurv";
    qsl+="Conic";

    if(pDevice==0)
        qsl+="Z";
    else
    {
        if(!pDevice->relative_convention())
        {
            qsl+="Z";
            m_ui->comboCoordMode->setCurrentIndex(0);
        }
        else
        {

            qsl+="Thick";
            m_ui->comboCoordMode->setCurrentIndex(1);

        }
    }

    qsl+="Diameter";


    // update inner diameter checkbox
    if(_bDisplayInnerDiameter )
    {
        qsl+="InnerDiameter";
    }
    m_ui->cbInnerDiameter->setChecked( _bDisplayInnerDiameter);
    m_ui->cbInnerDiameter->setEnabled(!_bHaveInnerDiameter );

    // update aspheric checkbox
    if(_bDisplayAspheric )
    {
        qsl+="r4";
        qsl+="r6";
        qsl+="r8";
        qsl+="r10";
    }
    m_ui->cbPolyAspheric->setChecked( _bDisplayAspheric);
    m_ui->cbPolyAspheric->setEnabled(!_bHaveAspheric );

    // update comment checkbox
    if(_bDisplayComment )
    {
        qsl+="Comment";
    }

    m_ui->cbComment->setChecked( _bDisplayComment);
    m_ui->cbComment->setEnabled(!_bHaveComment );

    m_ui->twSurfacesDatas->setColumnCount(qsl.size());
    m_ui->twSurfacesDatas->setHorizontalHeaderLabels (qsl);
}
/////////////////////////////////////////////////////////////////////////://///
DockSurfacesData::~DockSurfacesData()
{
    delete m_ui;
}
/////////////////////////////////////////////////////////////////////////://///
void DockSurfacesData::changeEvent(QEvent *e)
{
    QDockWidget::changeEvent(e);
    switch (e->type())
    {
    case QEvent::LanguageChange:
        m_ui->retranslateUi(this);
        break;
    default:
        break;
    }
}
//////////////////////////////////////////////////////////////////////////////
void DockSurfacesData::device_changed(OpticalDevice *pDevice,int iReason)
{
    assert(pDevice!=0);
    pOD=pDevice;

    _bCanEmit=false;

    if(iReason==NEW_OPTICAL_DEVICE)
    {
        m_ui->twSurfacesDatas->clearContents();
        m_ui->twSurfacesDatas->setRowCount(pDevice->nb_surface());

        _bDisplayComment=false;
        _bDisplayAspheric=false;
        _bDisplayInnerDiameter=false;
    }

    _bHaveInnerDiameter=pDevice->has_inner_diameter();
    if(_bHaveInnerDiameter)
        _bDisplayInnerDiameter=true;

    _bHaveAspheric=pDevice->has_aspheric();
    if(_bHaveAspheric)
        _bDisplayAspheric=true;

    _bHaveComment=pDevice->has_comment();
    if(_bHaveComment)
        _bDisplayComment=true;

    update_labels(pDevice);
    update_table();
}
/////////////////////////////////////////////////////////////////////////://///
void DockSurfacesData::update_table()
{
    int iRefSurfAlias;
    _bCanEmit=false;

    vector<string> vsMaterial;
    GlassManager::singleton().list_available(vsMaterial);

    m_ui->twSurfacesDatas->setRowCount(pOD->nb_surface());

    for (int i=0;i<pOD->nb_surface();i++)
    {
        int iIndexCol=0;
        QComboBox*  qcbType=new QComboBox;
        qcbType->addItem("stop");
        qcbType->addItem("reflect");
        qcbType->addItem("void");
        qcbType->addItem("image");
        qcbType->addItem("perfect_lens");
        qcbType->addItem("perfect_mirror");
        qcbType->addItem("image");
        for(unsigned int iM=0;iM<vsMaterial.size();iM++) //TODO optimiser
            qcbType->addItem(vsMaterial[iM].c_str());

        qcbType->insertSeparator(7);

        string sType=pOD->type(i);
        int iPosType=qcbType->findText(sType.c_str());
        if(iPosType==-1)
        {
            //non existent glass : TODO add warning
            qcbType->addItem(sType.c_str());
            qcbType->setCurrentText(sType.c_str());
        }
        else
            qcbType->setCurrentIndex(iPosType);

        m_ui->twSurfacesDatas->setCellWidget(i,ITEM_TYPE,qcbType);
        connect(qcbType,SIGNAL(activated(int)),this,SLOT(onTypeChanged()));

        // radius curvature
        double dRC=pOD->get(i,RADIUS_CURVATURE);
        QString qsRadius="inf";
        if( (dRC<RADIUS_CURVATURE_INFINITY/2.) && (dRC>-RADIUS_CURVATURE_INFINITY/2.) )
            qsRadius=QString::number(dRC,'g',10);
        if(pOD->get_image_autocurvature() && (i==pOD->nb_surface()-1) )
            qsRadius="auto "+qsRadius;
        if(pOD->get_alias(i,RADIUS_CURVATURE,iRefSurfAlias))
            qsRadius="#"+QString::number(i)+" "+qsRadius;
        m_ui->twSurfacesDatas->setItem(i,1, new QTableWidgetItem(qsRadius));

        //conic
        double dConic=pOD->get(i,CONIC);
        QString qsConic=QString::number(dConic,'g',10);
        if(pOD->get_alias(i,CONIC,iRefSurfAlias))
            qsConic="#"+QString::number(i)+" "+qsConic;
        m_ui->twSurfacesDatas->setItem(i,2, new QTableWidgetItem(qsConic));

        // z or thick
        double dZ;
        if(!pOD->relative_convention())
            dZ=pOD->get(i,Z);
        else
            dZ=pOD->get(i,THICK);
        QString qsZ=QString::number(dZ,'g',10);
        if(pOD->get_autofocus() && (i==pOD->nb_surface()-1) )
            qsZ="auto "+qsZ;
        m_ui->twSurfacesDatas->setItem(i,3,new QTableWidgetItem(qsZ));

        //diameter
        double dDiameter=pOD->get(i,DIAMETER);
        QString qsDiameter="";
        if(pOD->get(i,AUTO_DIAMETER)!=0.)
            qsDiameter="auto "+QString::number(dDiameter,'f',3);
        else
            qsDiameter=QString::number(dDiameter,'g',10);

        m_ui->twSurfacesDatas->setItem(i,4,new QTableWidgetItem(qsDiameter));

        iIndexCol=5;
        if(_bDisplayInnerDiameter)
        {
            double dInnerDiameter=pOD->get(i,INNER_DIAMETER);
            QString qsInnerDiameter="";
            if(pOD->get(i,AUTO_INNER_DIAMETER)!=0.)
                qsInnerDiameter="auto "+QString::number(dInnerDiameter,'f',3);
            else
                qsInnerDiameter=QString::number(dInnerDiameter,'g',10);

            m_ui->twSurfacesDatas->setItem(i,iIndexCol,new QTableWidgetItem(qsInnerDiameter));

            iIndexCol++;
        }

        if(_bDisplayAspheric)
        {
            double dR4=pOD->get(i,R4);
            double dR6=pOD->get(i,R6);
            double dR8=pOD->get(i,R8);
            double dR10=pOD->get(i,R10);

            m_ui->twSurfacesDatas->setItem(i,iIndexCol,new QTableWidgetItem(QString::number(dR4,'g',10)));
            m_ui->twSurfacesDatas->setItem(i,iIndexCol+1,new QTableWidgetItem(QString::number(dR6,'g',10)));
            m_ui->twSurfacesDatas->setItem(i,iIndexCol+2,new QTableWidgetItem(QString::number(dR8,'g',10)));
            m_ui->twSurfacesDatas->setItem(i,iIndexCol+3,new QTableWidgetItem(QString::number(dR10,'g',10)));

            iIndexCol+=4;
        }

        if(_bDisplayComment)
        {
            string sComment=pOD->comment(i);
            m_ui->twSurfacesDatas->setItem(i,iIndexCol,new QTableWidgetItem(sComment.c_str()));

            //      iIndexCol+=1; //
        }
    }

    m_ui->twSurfacesDatas->resizeColumnsToContents();

    _bCanEmit=true;
}
//////////////////////////////////////////////////////////////////////////////
void DockSurfacesData::onTypeChanged()
{
    //todo remove this function?
    for(int i=0;i<pOD->nb_surface();i++)
    {
        QComboBox* pComboBox=dynamic_cast<QComboBox*>(m_ui->twSurfacesDatas->cellWidget(i,ITEM_TYPE));
        assert(pComboBox!=0);
        string sType=pOD->type(i);
        if(pComboBox->currentText()!=sType.c_str())
            OnCellChanged(i,ITEM_TYPE);
    }
}
//////////////////////////////////////////////////////////////////////////////
void DockSurfacesData::OnCellChanged(int iRow,int iCol)
{
    if(!_bCanEmit)
        return;

    if (iCol==ITEM_TYPE) //"type"
    {
        QComboBox* pComboBox=dynamic_cast<QComboBox*>(m_ui->twSurfacesDatas->cellWidget(iRow,iCol));
        assert(pComboBox!=0);
        QString qsText=pComboBox->currentText();
        string sType=qsText.toStdString();
        pOD->set_type(iRow,sType);
    }

    if (iCol==1) //"radius_curvature"
    {
        string sItem=m_ui->twSurfacesDatas->item(iRow,iCol)->text().toStdString();
        bool bAuto=sItem.find("auto")!=string::npos;
        bool bInf=sItem.find("inf")!=string::npos;
        bool bAlias=sItem.find("#")!=string::npos;
        bool bLastSurf=iRow==pOD->nb_surface()-1;

        if(bAlias)
        {
            sItem[sItem.find("#")]=' '; //TODO something more robust
            stringstream ss(sItem);
            int iVal=0;
            ss >> iVal;
            pOD->set_alias(iRow,RADIUS_CURVATURE,iVal);
        }
        else
        {
            if(bAuto && bLastSurf)
                pOD->set_image_autocurvature(bAuto);
            else
            {
                if(bLastSurf)
                    pOD->set_image_autocurvature(false);

                if(bInf)
                    pOD->set(iRow,RADIUS_CURVATURE,RADIUS_CURVATURE_INFINITY);
                else
                {
                    if(bAuto)
                        sItem=sItem.substr(sItem.find("auto")+4,string::npos);

                    stringstream ss(sItem);
                    double dVal=0;
                    ss >> dVal;
                    pOD->set(iRow,RADIUS_CURVATURE,dVal);
                }
            }
        }
    }

    if (iCol==2) //"conic"
    {
        string sItem=m_ui->twSurfacesDatas->item(iRow,iCol)->text().toStdString();
        bool bAlias=sItem.find("#")!=string::npos;
        if(bAlias)
        {
            sItem[sItem.find("#")]=' '; //TODO something more robust
            stringstream ss(sItem);
            int iVal=0;
            ss >> iVal;
            pOD->set_alias(iRow,CONIC,iVal);
        }
        else
        {
            stringstream ss(sItem);
            double dVal=0;
            ss >> dVal;
            pOD->set(iRow,CONIC,dVal);
        }
    }

    if (iCol==3) //"z" or "thick"
    {
        string sItem=m_ui->twSurfacesDatas->item(iRow,iCol)->text().toStdString();
        bool bAuto=sItem.find("auto")!=string::npos;
        if(bAuto)
            sItem=sItem.substr(sItem.find("auto")+4,string::npos);

        stringstream ss(sItem);
        double dVal=0;
        ss >> dVal;

        if(pOD->relative_convention())
            pOD->set(iRow,THICK,dVal);
        else
            pOD->set(iRow,Z,dVal);

        if(iRow==pOD->nb_surface()-1)
            pOD->set_autofocus(bAuto);
    }

    if (iCol==4) //"Diameter"
    {
        string sItem=m_ui->twSurfacesDatas->item(iRow,iCol)->text().toStdString();
        bool bAuto=sItem.find("auto")!=string::npos;
        if(bAuto)
            sItem=sItem.substr(sItem.find("auto")+4,string::npos);

        stringstream ss(sItem);
        double dVal=0;
        ss >> dVal;

        pOD->set(iRow,AUTO_DIAMETER,bAuto);
        pOD->set(iRow,DIAMETER,dVal);
    }

    int iIndexCol=5;
    if(_bDisplayInnerDiameter)
    {
        if (iCol==iIndexCol)
        {
            string sItem=m_ui->twSurfacesDatas->item(iRow,iCol)->text().toStdString();
            bool bAuto=sItem.find("auto")!=string::npos;
            if(bAuto)
                sItem=sItem.substr(sItem.find("auto")+4,string::npos);

            stringstream ss(sItem);
            double dVal;
            ss >> dVal;

            pOD->set(iRow,AUTO_INNER_DIAMETER,bAuto);
            pOD->set(iRow,INNER_DIAMETER,dVal);
        }

        iIndexCol++;
    }

    if(_bDisplayAspheric)
    {
        if (iCol==iIndexCol)
        {
            QTableWidgetItem* pItem=m_ui->twSurfacesDatas->item(iRow,iCol);
            pOD->set(iRow,R4,pItem->text().toDouble());
        }

        if (iCol==iIndexCol+1)
        {
            QTableWidgetItem* pItem=m_ui->twSurfacesDatas->item(iRow,iCol);
            pOD->set(iRow,R6,pItem->text().toDouble());
        }

        if (iCol==iIndexCol+2)
        {
            QTableWidgetItem* pItem=m_ui->twSurfacesDatas->item(iRow,iCol);
            pOD->set(iRow,R8,pItem->text().toDouble());
        }

        if (iCol==iIndexCol+3)
        {
            QTableWidgetItem* pItem=m_ui->twSurfacesDatas->item(iRow,iCol);
            pOD->set(iRow,R10,pItem->text().toDouble());
        }

        iIndexCol+=4;
    }

    if((iCol==iIndexCol) && _bDisplayComment)
    {
        QTableWidgetItem* pItem=m_ui->twSurfacesDatas->item(iRow,iCol);
        QString qsText=pItem->text();

        pOD->set_comment(iRow,qsText.toStdString());
        //    iCol++;
    }

    static_cast<MainWindow*>(parent())->update_views(this,OPTICAL_DEVICE_CHANGED);
}
//////////////////////////////////////////////////////////////////////////////
void DockSurfacesData::OnAddSurfaceAfter()
{
    int iLine=m_ui->twSurfacesDatas->currentRow();
    if(iLine==-1)
    {
        if(pOD->nb_surface()!=0)
            return;
    }

    pOD->insert_surface(iLine+1);
    pOD->set_type(iLine+1,"void");

    if(iLine+1>0)
        pOD->set(iLine+1,AUTO_DIAMETER,true);

    static_cast<MainWindow*>(parent())->update_views(this,OPTICAL_DEVICE_CHANGED);

    m_ui->twSurfacesDatas->selectRow(iLine+1);
}
//////////////////////////////////////////////////////////////////////////////
void DockSurfacesData::OnAddSurfaceBefore()
{
    int iLine=m_ui->twSurfacesDatas->currentRow();
    if(iLine==-1)
    {
        if(pOD->nb_surface()!=0)
            return;
        iLine=0;
    }

    pOD->insert_surface(iLine);
    pOD->set_type(iLine,"void");

    if(iLine>0)
        pOD->set(iLine,AUTO_DIAMETER,true);

    static_cast<MainWindow*>(parent())->update_views(this,OPTICAL_DEVICE_CHANGED);

    m_ui->twSurfacesDatas->selectRow(iLine);
}
//////////////////////////////////////////////////////////////////////////////
void DockSurfacesData::OnDeleteSurface()
{
    int iLine=m_ui->twSurfacesDatas->currentRow();
    if(iLine==-1)
        return;

    pOD->delete_surface(iLine);
    static_cast<MainWindow*>(parent())->update_views(this,OPTICAL_DEVICE_CHANGED);

    if(iLine>=m_ui->twSurfacesDatas->rowCount())
        iLine =m_ui->twSurfacesDatas->rowCount()-1;

    if(iLine<0)
        iLine=0;

    m_ui->twSurfacesDatas->selectRow(iLine);
}
//////////////////////////////////////////////////////////////////////////////
void DockSurfacesData::on_cbPolyAspheric_clicked()
{
    if(!_bCanEmit)
        return;

    _bDisplayAspheric=m_ui->cbPolyAspheric->isChecked();
    static_cast<MainWindow*>(parent())->update_views(this,USER_INTERFACE_CHANGED);
}
//////////////////////////////////////////////////////////////////////////////
void DockSurfacesData::on_cbInnerDiameter_clicked()
{
    if(!_bCanEmit)
        return;

    _bDisplayInnerDiameter=m_ui->cbInnerDiameter->isChecked();
    static_cast<MainWindow*>(parent())->update_views(this,USER_INTERFACE_CHANGED);
}
//////////////////////////////////////////////////////////////////////////////
void DockSurfacesData::on_cbComment_clicked()
{
    if(!_bCanEmit)
        return;

    _bDisplayComment=m_ui->cbComment->isChecked();
    static_cast<MainWindow*>(parent())->update_views(this,USER_INTERFACE_CHANGED);
}
//////////////////////////////////////////////////////////////////////////////
void DockSurfacesData::on_comboCoordMode_activated(const QString &arg1)
{
    (void)arg1;

    int iSelected=m_ui->comboCoordMode->currentIndex();

    if(iSelected==0)
        pOD->set_relative_convention(false);

    if(iSelected==1)
        pOD->set_relative_convention(true);

    static_cast<MainWindow*>(parent())->update_views(this,OPTICAL_DEVICE_CHANGED);
}
//////////////////////////////////////////////////////////////////////////////
