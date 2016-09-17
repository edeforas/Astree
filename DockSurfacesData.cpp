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
    _bBlockSignals=true;

    _pOD=0;
    m_ui->setupUi(this);

    m_ui->twSurfacesDatas->setColumnCount(8);
    update_labels();

    _bHaveAspheric=false;
    _bDisplayAspheric=false;

    _bHaveInnerDiameter=false;
    _bDisplayInnerDiameter=false;

    _bHaveComment=false;
    _bDisplayComment=false;

    _bBlockSignals=false;
}
/////////////////////////////////////////////////////////////////////////://///
void DockSurfacesData::update_labels()
{
    QStringList qsl;
    qsl+="Type";
    qsl+="RCurv";
    qsl+="Conic";

    if(_pOD==0)
        qsl+="Z";
    else
    {
        if(!_pOD->relative_convention())
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
    if(_pOD)
        _bHaveInnerDiameter=_pOD->has_inner_diameter();
    else
        _bHaveInnerDiameter=false;

    if(_bHaveInnerDiameter)
        _bDisplayInnerDiameter=true;

    if(_bDisplayInnerDiameter )
    {
        qsl+="InnerDiameter";
    }
    m_ui->cbInnerDiameter->setChecked( _bDisplayInnerDiameter);
    m_ui->cbInnerDiameter->setEnabled(!_bHaveInnerDiameter );

    // update aspheric checkbox
    if(_pOD)
        _bHaveAspheric=_pOD->has_aspheric();
    else
        _bHaveAspheric=false;

    if(_bHaveAspheric)
        _bDisplayAspheric=true;

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
    if(_pOD)
        _bHaveComment=_pOD->has_comment();
    else
        _bHaveComment=false;

    if(_bHaveComment)
        _bDisplayComment=true;

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
    _pOD=pDevice;

    if( (iReason!=OPTICAL_DEVICE_CHANGED) && (iReason!=NEW_OPTICAL_DEVICE) )
        return;

    _bBlockSignals=true;

    if(iReason==NEW_OPTICAL_DEVICE)
    {
        m_ui->twSurfacesDatas->clearContents();
        m_ui->twSurfacesDatas->setRowCount(pDevice->nb_surface());

        _bDisplayComment=false;
        _bDisplayAspheric=false;
        _bDisplayInnerDiameter=false;
    }

    update_labels();
    update_table();

    _bBlockSignals=false;
}
/////////////////////////////////////////////////////////////////////////://///
void DockSurfacesData::update_table()
{
    _bBlockSignals=true;

    vector<string> vsMaterial;
    GlassManager::singleton().list_available(vsMaterial);

    m_ui->twSurfacesDatas->setRowCount(_pOD->nb_surface());

    for (int i=0;i<_pOD->nb_surface();i++)
    {
        int iRefAlias;
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

        string sType=_pOD->type(i);
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
        double dRC=_pOD->get(i,RADIUS_CURVATURE);
        QString qsRadius="inf";
        if( (dRC<RADIUS_CURVATURE_INFINITY/2.) && (dRC>-RADIUS_CURVATURE_INFINITY/2.) )
            qsRadius=QString::number(dRC,'g',10);
        if(_pOD->get_image_autocurvature() && (i==_pOD->nb_surface()-1) )
            qsRadius="auto "+qsRadius;
        if(_pOD->get_alias(i,RADIUS_CURVATURE,iRefAlias))
            qsRadius="#"+QString::number(iRefAlias+1)+" "+qsRadius;
        m_ui->twSurfacesDatas->setItem(i,1, new QTableWidgetItem(qsRadius));

        //conic
        double dConic=_pOD->get(i,CONIC);
        QString qsConic=QString::number(dConic,'g',10);
        if(_pOD->get_alias(i,CONIC,iRefAlias))
            qsConic="#"+QString::number(iRefAlias+1)+" "+qsConic;
        m_ui->twSurfacesDatas->setItem(i,2, new QTableWidgetItem(qsConic));

        // z or thick
        double dZ;
        if(!_pOD->relative_convention())
            dZ=_pOD->get(i,Z);
        else
            dZ=_pOD->get(i,THICK);
        QString qsZ=QString::number(dZ,'g',10);
        if(_pOD->get_autofocus() && (i==_pOD->nb_surface()-1) )
            qsZ="auto "+qsZ;
        m_ui->twSurfacesDatas->setItem(i,3,new QTableWidgetItem(qsZ));

        //diameter
        double dDiameter=_pOD->get(i,DIAMETER);
        QString qsDiameter="";
        if(_pOD->get(i,AUTO_DIAMETER)!=0.)
            qsDiameter="auto "+QString::number(dDiameter,'f',3);
        else
            qsDiameter=QString::number(dDiameter,'g',10);

        m_ui->twSurfacesDatas->setItem(i,4,new QTableWidgetItem(qsDiameter));

        iIndexCol=5;
        if(_bDisplayInnerDiameter)
        {
            double dInnerDiameter=_pOD->get(i,INNER_DIAMETER);
            QString qsInnerDiameter="";
            if(_pOD->get(i,AUTO_INNER_DIAMETER)!=0.)
                qsInnerDiameter="auto "+QString::number(dInnerDiameter,'f',3);
            else
                qsInnerDiameter=QString::number(dInnerDiameter,'g',10);

            m_ui->twSurfacesDatas->setItem(i,iIndexCol,new QTableWidgetItem(qsInnerDiameter));

            iIndexCol++;
        }

        if(_bDisplayAspheric)
        {
            double dR4=_pOD->get(i,R4);
            double dR6=_pOD->get(i,R6);
            double dR8=_pOD->get(i,R8);
            double dR10=_pOD->get(i,R10);

            m_ui->twSurfacesDatas->setItem(i,iIndexCol,new QTableWidgetItem(QString::number(dR4,'g',10)));
            m_ui->twSurfacesDatas->setItem(i,iIndexCol+1,new QTableWidgetItem(QString::number(dR6,'g',10)));
            m_ui->twSurfacesDatas->setItem(i,iIndexCol+2,new QTableWidgetItem(QString::number(dR8,'g',10)));
            m_ui->twSurfacesDatas->setItem(i,iIndexCol+3,new QTableWidgetItem(QString::number(dR10,'g',10)));

            iIndexCol+=4;
        }

        if(_bDisplayComment)
        {
            string sComment=_pOD->comment(i);
            m_ui->twSurfacesDatas->setItem(i,iIndexCol,new QTableWidgetItem(sComment.c_str()));

            //      iIndexCol+=1; //
        }
    }

    m_ui->twSurfacesDatas->resizeColumnsToContents();
    _bBlockSignals=false;
}
//////////////////////////////////////////////////////////////////////////////
void DockSurfacesData::onTypeChanged()
{
    if(_bBlockSignals)
        return;

    //todo remove this function?
    for(int i=0;i<_pOD->nb_surface();i++)
    {
        QComboBox* pComboBox=dynamic_cast<QComboBox*>(m_ui->twSurfacesDatas->cellWidget(i,ITEM_TYPE));
        assert(pComboBox!=0);
        string sType=_pOD->type(i);
        if(pComboBox->currentText()!=sType.c_str())
            on_twSurfacesDatas_cellChanged(i,ITEM_TYPE);
    }
}
//////////////////////////////////////////////////////////////////////////////
void DockSurfacesData::on_twSurfacesDatas_cellChanged(int iRow, int iCol)
{
    if(_bBlockSignals)
        return;

    if (iCol==ITEM_TYPE) //"type"
    {
        QComboBox* pComboBox=dynamic_cast<QComboBox*>(m_ui->twSurfacesDatas->cellWidget(iRow,iCol));
        assert(pComboBox!=0);
        QString qsText=pComboBox->currentText();
        string sType=qsText.toStdString();
        _pOD->set_type(iRow,sType);
    }

    if (iCol==1) //"radius_curvature"
    {
        string sItem=m_ui->twSurfacesDatas->item(iRow,iCol)->text().toStdString();
        bool bAuto=sItem.find("auto")!=string::npos;
        bool bInf=sItem.find("inf")!=string::npos;
        bool bAlias=sItem.find("#")!=string::npos;
        bool bLastSurf=iRow==_pOD->nb_surface()-1;

        if(bAlias)
        {
            sItem[sItem.find("#")]=' '; //TODO something more robust
            stringstream ss(sItem);
            int iVal=0;
            ss >> iVal;
            _pOD->set_alias(iRow,RADIUS_CURVATURE,iVal-1);
        }
        else
        {
            _pOD->set_alias(iRow,RADIUS_CURVATURE,-1);
            if(bAuto && bLastSurf)
                _pOD->set_image_autocurvature(bAuto);
            else
            {
                if(bLastSurf)
                    _pOD->set_image_autocurvature(false);

                if(bInf)
                    _pOD->set(iRow,RADIUS_CURVATURE,RADIUS_CURVATURE_INFINITY);
                else
                {
                    if(bAuto)
                        sItem=sItem.substr(sItem.find("auto")+4,string::npos);

                    stringstream ss(sItem);
                    double dVal=0;
                    ss >> dVal;
                    _pOD->set(iRow,RADIUS_CURVATURE,dVal);
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
            _pOD->set_alias(iRow,CONIC,iVal-1);
        }
        else
        {
            _pOD->set_alias(iRow,CONIC,-1);
            stringstream ss(sItem);
            double dVal=0;
            ss >> dVal;
            _pOD->set(iRow,CONIC,dVal);
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

        if(_pOD->relative_convention())
            _pOD->set(iRow,THICK,dVal);
        else
            _pOD->set(iRow,Z,dVal);

        if(iRow==_pOD->nb_surface()-1)
            _pOD->set_autofocus(bAuto);
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

        _pOD->set(iRow,AUTO_DIAMETER,bAuto);
        _pOD->set(iRow,DIAMETER,dVal);
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

            _pOD->set(iRow,AUTO_INNER_DIAMETER,bAuto);
            _pOD->set(iRow,INNER_DIAMETER,dVal);
            update_labels();
        }

        iIndexCol++;
    }

    if(_bDisplayAspheric)
    {
        if (iCol==iIndexCol)
        {
            QTableWidgetItem* pItem=m_ui->twSurfacesDatas->item(iRow,iCol);
            _pOD->set(iRow,R4,pItem->text().toDouble());
            update_labels();
        }

        if (iCol==iIndexCol+1)
        {
            QTableWidgetItem* pItem=m_ui->twSurfacesDatas->item(iRow,iCol);
            _pOD->set(iRow,R6,pItem->text().toDouble());
            update_labels();
        }

        if (iCol==iIndexCol+2)
        {
            QTableWidgetItem* pItem=m_ui->twSurfacesDatas->item(iRow,iCol);
            _pOD->set(iRow,R8,pItem->text().toDouble());
            update_labels();
        }

        if (iCol==iIndexCol+3)
        {
            QTableWidgetItem* pItem=m_ui->twSurfacesDatas->item(iRow,iCol);
            _pOD->set(iRow,R10,pItem->text().toDouble());
            update_labels();
        }

        iIndexCol+=4;
    }

    if((iCol==iIndexCol) && _bDisplayComment)
    {
        QTableWidgetItem* pItem=m_ui->twSurfacesDatas->item(iRow,iCol);
        QString qsText=pItem->text();

        _pOD->set_comment(iRow,qsText.toStdString());
        update_labels();

        //    iCol++;
    }

    if(_pOD->has_auto())
        update_table(); // an auto parameter need to be recomputed

    static_cast<MainWindow*>(parent())->device_changed(this,OPTICAL_DEVICE_CHANGED);
}

//////////////////////////////////////////////////////////////////////////////
void DockSurfacesData::on_cbPolyAspheric_clicked()
{
    if(_bBlockSignals)
        return;

    _bDisplayAspheric=m_ui->cbPolyAspheric->isChecked();
    update_labels();
    update_table();
    static_cast<MainWindow*>(parent())->device_changed(this,USER_INTERFACE_CHANGED);
}
//////////////////////////////////////////////////////////////////////////////
void DockSurfacesData::on_cbInnerDiameter_clicked()
{
    if(_bBlockSignals)
        return;

    _bDisplayInnerDiameter=m_ui->cbInnerDiameter->isChecked();
    update_labels();
    update_table();
    static_cast<MainWindow*>(parent())->device_changed(this,USER_INTERFACE_CHANGED);
}
//////////////////////////////////////////////////////////////////////////////
void DockSurfacesData::on_cbComment_clicked()
{
    if(_bBlockSignals)
        return;

    _bDisplayComment=m_ui->cbComment->isChecked();
    update_labels();
    update_table();
    static_cast<MainWindow*>(parent())->device_changed(this,USER_INTERFACE_CHANGED);
}
//////////////////////////////////////////////////////////////////////////////
void DockSurfacesData::on_comboCoordMode_activated(const QString &arg1)
{
    if(_bBlockSignals)
        return;

    (void)arg1;

    int iSelected=m_ui->comboCoordMode->currentIndex();

    if(iSelected==0)
        _pOD->set_relative_convention(false);

    if(iSelected==1)
        _pOD->set_relative_convention(true);

    update_table();
    static_cast<MainWindow*>(parent())->device_changed(this,OPTICAL_DEVICE_CHANGED);
}
//////////////////////////////////////////////////////////////////////////////
void DockSurfacesData::on_btnAddSurfaceBefore_clicked()
{
    int iLine=m_ui->twSurfacesDatas->currentRow();
    if(iLine==-1)
    {
        if(_pOD->nb_surface()!=0)
            return;
        iLine=0;
    }

    _pOD->insert_surface(iLine);
    _pOD->set_type(iLine,"void");

    if(iLine>0)
        _pOD->set(iLine,AUTO_DIAMETER,true);

    update_table(); // a auto parameter need to be recomputed

    static_cast<MainWindow*>(parent())->device_changed(this,OPTICAL_DEVICE_CHANGED);

    m_ui->twSurfacesDatas->selectRow(iLine);
}
//////////////////////////////////////////////////////////////////////////////
void DockSurfacesData::on_btnAddSurfaceAfter_clicked()
{
    int iLine=m_ui->twSurfacesDatas->currentRow();
    if(iLine==-1)
    {
        if(_pOD->nb_surface()!=0)
            return;
    }

    _pOD->insert_surface(iLine+1);
    _pOD->set_type(iLine+1,"void");

    if(iLine+1>0)
        _pOD->set(iLine+1,AUTO_DIAMETER,true);

    update_table(); // a auto parameter need to be recomputed

    static_cast<MainWindow*>(parent())->device_changed(this,OPTICAL_DEVICE_CHANGED);

    m_ui->twSurfacesDatas->selectRow(iLine+1);
}
//////////////////////////////////////////////////////////////////////////////
void DockSurfacesData::on_btnDeleteSurface_clicked()
{
    int iLine=m_ui->twSurfacesDatas->currentRow();
    if(iLine==-1)
        return;

    _pOD->delete_surface(iLine);
    static_cast<MainWindow*>(parent())->device_changed(this,OPTICAL_DEVICE_CHANGED);

    if(iLine>=m_ui->twSurfacesDatas->rowCount())
        iLine =m_ui->twSurfacesDatas->rowCount()-1;

    if(iLine<0)
        iLine=0;

    update_table(); // a auto parameter need to be recomputed

    m_ui->twSurfacesDatas->selectRow(iLine);
}
//////////////////////////////////////////////////////////////////////////////
