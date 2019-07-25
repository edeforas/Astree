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
        qsl+="R4";
        qsl+="R6";
        qsl+="R8";
        qsl+="R10";
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
        int iRefClone;
        double dCloneGain;
        int iIndexCol=0;
        QComboBox*  qcbType=new QComboBox;
        qcbType->addItem("stop");
        qcbType->addItem("reflect");
        qcbType->addItem("void");
        qcbType->addItem("image");
        qcbType->addItem("image_infinite");
        qcbType->addItem("perfect_lens");
        qcbType->addItem("perfect_mirror");
        qcbType->addItem("image");
        qcbType->addItem("glass...");
        for(unsigned int iM=0;iM<vsMaterial.size();iM++) //TODO optimize
            qcbType->addItem(vsMaterial[iM].c_str());

        qcbType->insertSeparator(9);

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
        if(_pOD->get_clone(i,RADIUS_CURVATURE,iRefClone,dCloneGain))
        {
            qsRadius="#"+QString::number(iRefClone+1)+" "+qsRadius;
            if(dCloneGain<0.)
                qsRadius="-"+qsRadius;
        }
        m_ui->twSurfacesDatas->setItem(i,1, new QTableWidgetItem(qsRadius));

        //conic
        double dConic=_pOD->get(i,CONIC);
        QString qsConic=QString::number(dConic,'g',10);
        if(_pOD->get_clone(i,CONIC,iRefClone,dCloneGain)) // TODO
            qsConic="#"+QString::number(iRefClone+1)+" "+qsConic;
        m_ui->twSurfacesDatas->setItem(i,2, new QTableWidgetItem(qsConic));

        // update Z or thick
        double dZ;
        eSurfaceParameter esp;
        if(!_pOD->relative_convention())
            esp=Z;
        else
            esp=THICK;
        dZ=_pOD->get(i,esp);

        QString qsZ=QString::number(dZ,'g',10);
        if(_pOD->get_autofocus() && (i==_pOD->nb_surface()-1) )
            qsZ="auto "+qsZ;

        //Manage Z or THICK in clone
        if(_pOD->get_clone(i,esp,iRefClone,dCloneGain))
        {
            qsZ="#"+QString::number(iRefClone+1)+" "+qsZ;
            if(dCloneGain<0.)
                qsZ="-"+qsZ;
        }

        m_ui->twSurfacesDatas->setItem(i,3,new QTableWidgetItem(qsZ));

        // update diameter
        double dDiameter=_pOD->get(i,DIAMETER);
        QString qsDiameter="";
        if(_pOD->get(i,AUTO_DIAMETER)!=0.)
            qsDiameter="auto "+QString::number(dDiameter,'f',3);
        else
            qsDiameter=QString::number(dDiameter,'g',10);

        if(_pOD->get_clone(i,DIAMETER,iRefClone,dCloneGain))
        {
            qsDiameter="#"+QString::number(iRefClone+1)+" "+qsDiameter;
            if(dCloneGain<0.)
                qsDiameter="-"+qsDiameter;
        }
        m_ui->twSurfacesDatas->setItem(i,4,new QTableWidgetItem(qsDiameter));

        // update inner diameter
        iIndexCol=5;
        if(_bDisplayInnerDiameter)
        {
            double dInnerDiameter=_pOD->get(i,INNER_DIAMETER);
            QString qsInnerDiameter="";
            if(_pOD->get(i,AUTO_INNER_DIAMETER)!=0.)
                qsInnerDiameter="auto "+QString::number(dInnerDiameter,'f',3);
            else
                qsInnerDiameter=QString::number(dInnerDiameter,'g',10);

            if(_pOD->get_clone(i,INNER_DIAMETER,iRefClone,dCloneGain))
            {
                qsInnerDiameter="#"+QString::number(iRefClone+1)+" "+qsInnerDiameter;
                if(dCloneGain<0.)
                    qsInnerDiameter="-"+qsInnerDiameter;
            }
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

            //      iIndexCol+=1;
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

    for(int iRow=0;iRow<_pOD->nb_surface();iRow++)
    {
        QComboBox* pComboBox=dynamic_cast<QComboBox*>(m_ui->twSurfacesDatas->cellWidget(iRow,ITEM_TYPE));
        assert(pComboBox!=0);

        string sOldType=_pOD->type(iRow);
        string sNewType=pComboBox->currentText().toStdString();

        if(sNewType=="glass...")
        {
            _pOD->set_type(iRow,sOldType);
            update_table();
            static_cast<MainWindow*>(parent())->device_changed(this,CHANGE_GLASS);
            return;
        }

        if(sNewType!=sOldType)
            _pOD->set_type(iRow,sNewType);
    }
    update_table();
    static_cast<MainWindow*>(parent())->device_changed(this,OPTICAL_DEVICE_CHANGED);
}
//////////////////////////////////////////////////////////////////////////////
void DockSurfacesData::on_twSurfacesDatas_cellChanged(int iRow, int iCol)
{
    if(_bBlockSignals)
        return;

    //parse cell
    string sItem=m_ui->twSurfacesDatas->item(iRow,iCol)->text().toStdString();
    bool isAuto=sItem.find("auto")!=string::npos;
    bool isInf=sItem.find("inf")!=string::npos;
    double dGain=0.;
    int iSurfaceRef=-1;
    bool isClone=sItem.find("#")!=string::npos;
    bool isNegClone=sItem.find("-#")!=string::npos;
    bool isLastSurf=(iRow==_pOD->nb_surface()-1);
    bool isFirstSurf=(iRow==0);
    double dVal=-1.; //TODO set to NaN

    //check error and return last result if error
    if(isAuto && isFirstSurf)
    {
        update_table();
        return;
    }

    if(isAuto)
        sItem=sItem.erase(sItem.find("auto"),4);

    if(isInf)
    {
        sItem=sItem.erase(sItem.find("inf"),3);
        dVal=RADIUS_CURVATURE_INFINITY;
    }

    if(isClone)
    {
        sItem=sItem.substr(sItem.find("#")+1);
        stringstream ss(sItem);
        ss >> iSurfaceRef;
        iSurfaceRef--;
        if(isNegClone)
            dGain=-1.;
        else
            dGain=1.;
    }

    if(!isInf && !isAuto && !isClone)
    {
        stringstream ss(sItem);
        ss >> dVal;
    }

    if (iCol==1) //"radius_curvature"
    {
        _pOD->set_clone(iRow,RADIUS_CURVATURE,iSurfaceRef,dGain);
        if(!isClone)
        {
            _pOD->set(iRow,RADIUS_CURVATURE,dVal);
            if(isLastSurf)
                _pOD->set_image_autocurvature(isAuto);
        }
    }

    if (iCol==2) //"conic"
    {
        _pOD->set_clone(iRow,CONIC,iSurfaceRef,dGain);
        if(!isClone)
            _pOD->set(iRow,CONIC,dVal);
    }

    if (iCol==3) //"z" or "thick"
    {
        eSurfaceParameter esp;
        if(!_pOD->relative_convention())
            esp=Z;
        else
            esp=THICK;

        _pOD->set_clone(iRow,esp,iSurfaceRef,dGain);
        if(!isClone)
        {
            if( (esp==THICK) && isLastSurf)
                _pOD->set(iRow,esp,0.);
            else
                _pOD->set(iRow,esp,dVal);

            if(isLastSurf)
                _pOD->set_autofocus(isAuto);
        }
    }

    if (iCol==4) //"Diameter"
    {
        _pOD->set_clone(iRow,DIAMETER,iSurfaceRef,dGain);
        _pOD->set(iRow,AUTO_DIAMETER,isAuto && (!isFirstSurf) && (!isClone) );
        if((!isClone) && (!isAuto) )
            _pOD->set(iRow,DIAMETER,dVal); //TODO debug ref to auto diameter
    }

    int iIndexCol=5; // optional columns
    if(_bDisplayInnerDiameter)
    {
        if (iCol==iIndexCol) //inner diameter
        {
            _pOD->set_clone(iRow,INNER_DIAMETER,iSurfaceRef,dGain); //TODO debug ref to auto inner_diameter
            _pOD->set(iRow,AUTO_INNER_DIAMETER,isAuto && (!isFirstSurf) && (!isClone) );
            if((!isClone) && (!isAuto) )
                _pOD->set(iRow,INNER_DIAMETER,dVal);

            update_labels();
        }

        iIndexCol++;
    }

    if(_bDisplayAspheric)
    {
        if( (iCol>=iIndexCol) &&(iCol<=iIndexCol+3))
        {
            // no auto or clone for now

            eSurfaceParameter esp=R4;
            if(iCol==iIndexCol+1)
                esp=R6;

            if(iCol==iIndexCol+2)
                esp=R8;

            if(iCol==iIndexCol+3)
                esp=R10;

            QTableWidgetItem* pItem=m_ui->twSurfacesDatas->item(iRow,iCol);
            _pOD->set(iRow,esp,pItem->text().toDouble());
            update_labels();
        }

        iIndexCol+=4;
    }

    if(_bDisplayComment && (iCol==iIndexCol) )
    {
        QString qsText=m_ui->twSurfacesDatas->item(iRow,iCol)->text();
        _pOD->set_comment(iRow,qsText.toStdString());
        update_labels();

        //    iCol++;
    }

    update_table(); // other cells need to be recomputed
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

    update_labels();
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

    update_table(); // auto parameter need to be recomputed

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

    update_table(); // auto parameter need to be recomputed

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

    update_table(); // auto parameter need to be recomputed

    m_ui->twSurfacesDatas->selectRow(iLine);
}
//////////////////////////////////////////////////////////////////////////////
int DockSurfacesData::selected_surface() const
{
    return m_ui->twSurfacesDatas->currentRow();
}
//////////////////////////////////////////////////////////////////////////////
