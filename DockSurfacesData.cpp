// this file is covered by the General Public License version 2 or later
// please see GPL.html for more details and licensing issues
// copyright Etienne de Foras ( the author )  mailto: etienne.deforas@gmail.com

#include "DockSurfacesData.h"
#include "ui_DockSurfacesData.h"

#include <QComboBox>
#include <cassert>
#include <sstream>
#include "OpticalDevice.h"
#include "MaterialManager.h"
#include "MainWindow.h"
#include "AstreeDefines.h"

#define ITEM_TYPE 0


DockSurfacesData::DockSurfacesData(QWidget *parent) :
    QDockWidget(parent),
    m_ui(new Ui::DockSurfacesData)
{
    _pDevice=0;
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
        if(pDevice->convention()=="absolute")
        {
            qsl+="Z";
            m_ui->comboCoordMode->setCurrentText("Absolute");
        }
        else
        {

            qsl+="Thick";
            m_ui->comboCoordMode->setCurrentText("Relative");

        }}

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
/////////////////////////////////////////////////////////////////////////://///
void DockSurfacesData::device_changed(OpticalDevice *pDevice)
{
    assert(pDevice!=0);
    _pDevice=pDevice;

    _bCanEmit=false;

    if( (pDevice->nb_surface()!=_pDevice->nb_surface()) || (pDevice!=_pDevice) ) // TODO add cols count test and rewrite all tests here
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
    _bCanEmit=false;

    vector<string> vsMaterial;
    MaterialManager::singleton().list_available(vsMaterial);

    m_ui->twSurfacesDatas->setRowCount(_pDevice->nb_surface());

    for (int i=0;i<_pDevice->nb_surface();i++)
    {
        int iIndexCol=0;
        QComboBox*  qcbType=new QComboBox;
        qcbType->addItem("stop");
        qcbType->addItem("reflect");
        //   qcbType->addItem("transmit");
        qcbType->addItem("void");
        qcbType->addItem("image");
        for(unsigned int iM=0;iM<vsMaterial.size();iM++) //TODO optimiser
            qcbType->addItem(vsMaterial[iM].c_str());

        qcbType->insertSeparator(4);

        string sType=_pDevice->type(i);
        int iPosType=qcbType->findText(sType.c_str());
        assert(iPosType!=-1);
        qcbType->setCurrentIndex(iPosType);
        m_ui->twSurfacesDatas->setCellWidget(i,ITEM_TYPE,qcbType);
        connect(qcbType,SIGNAL(activated(int)),this,SLOT(onTypeChanged()));

        double dRC=_pDevice->get(i,RADIUS_CURVATURE);
        if( (dRC<RADIUS_CURVATURE_INFINITY/2.) && (dRC>-RADIUS_CURVATURE_INFINITY/2.) )
            m_ui->twSurfacesDatas->setItem(i,1, new QTableWidgetItem(QString::number(dRC,'g',10)));
        else
            m_ui->twSurfacesDatas->setItem(i,1, new QTableWidgetItem("inf"));

        double dConic=_pDevice->get(i,CONIC);
        m_ui->twSurfacesDatas->setItem(i,2, new QTableWidgetItem(QString::number(dConic,'g',10)));

        double dZ=_pDevice->z(i);
        QString qsZ=QString::number(dZ,'g',10);
        if(_pDevice->get_autofocus(i))
            qsZ="auto "+qsZ;
        m_ui->twSurfacesDatas->setItem(i,3,new QTableWidgetItem(qsZ));

        double dDiameter=_pDevice->get(i,DIAMETER);
        QString qsDiameter="";
        if(_pDevice->get(i,AUTO_DIAMETER))
            qsDiameter="auto "+QString::number(dDiameter,'f',3);
        else
            qsDiameter=QString::number(dDiameter,'g',10);

        m_ui->twSurfacesDatas->setItem(i,4,new QTableWidgetItem(qsDiameter));

        iIndexCol=5;
        if(_bDisplayInnerDiameter)
        {
            double dInnerDiameter=_pDevice->get(i,INNER_DIAMETER);
            QString qsInnerDiameter="";
            if(_pDevice->get(i,AUTO_INNER_DIAMETER))
                qsInnerDiameter="auto "+QString::number(dInnerDiameter,'f',3);
            else
                qsInnerDiameter=QString::number(dInnerDiameter,'g',10);

            m_ui->twSurfacesDatas->setItem(i,iIndexCol,new QTableWidgetItem(qsInnerDiameter));

            iIndexCol++;
        }

        if(_bDisplayAspheric)
        {
            double dR4=_pDevice->get(i,R4);
            double dR6=_pDevice->get(i,R6);
            double dR8=_pDevice->get(i,R8);
            double dR10=_pDevice->get(i,R10);

            m_ui->twSurfacesDatas->setItem(i,iIndexCol,new QTableWidgetItem(QString::number(dR4,'g',10)));
            m_ui->twSurfacesDatas->setItem(i,iIndexCol+1,new QTableWidgetItem(QString::number(dR6,'g',10)));
            m_ui->twSurfacesDatas->setItem(i,iIndexCol+2,new QTableWidgetItem(QString::number(dR8,'g',10)));
            m_ui->twSurfacesDatas->setItem(i,iIndexCol+3,new QTableWidgetItem(QString::number(dR10,'g',10)));

            iIndexCol+=4;
        }

        if(_bDisplayComment)
        {
            string sComment=_pDevice->comment(i);
            m_ui->twSurfacesDatas->setItem(i,iIndexCol,new QTableWidgetItem(sComment.c_str()));

            iIndexCol+=1;
        }
    }

    m_ui->twSurfacesDatas->resizeColumnsToContents();

    _bCanEmit=true;
}
//////////////////////////////////////////////////////////////////////////////
void DockSurfacesData::onTypeChanged()
{
    //todo remove this fcn?
    for(int i=0;i<_pDevice->nb_surface();i++)
    {
        QComboBox* pComboBox=dynamic_cast<QComboBox*>(m_ui->twSurfacesDatas->cellWidget(i,ITEM_TYPE));
        assert(pComboBox!=0);
        string sType=_pDevice->type(i);
        if(pComboBox->currentText()!=sType.c_str())
            OnCellChanged(i,ITEM_TYPE);
    }
}
//////////////////////////////////////////////////////////////////////////////
void DockSurfacesData::OnCellChanged(int iRow,int iCol)
{
    if(_bCanEmit==false)
        return;

    if (iCol==ITEM_TYPE) //"type"
    {
        QComboBox* pComboBox=dynamic_cast<QComboBox*>(m_ui->twSurfacesDatas->cellWidget(iRow,iCol));
        assert(pComboBox!=0);
        QString qsText=pComboBox->currentText();
        string sType=qsText.toStdString();
        _pDevice->set_type(iRow,sType);
    }

    if (iCol==1) //"radius_curvature"
    {
        QTableWidgetItem* pItem=m_ui->twSurfacesDatas->item(iRow,iCol);
        QString qsText=pItem->text();

        if (qsText=="inf")
            _pDevice->set(iRow,RADIUS_CURVATURE,RADIUS_CURVATURE_INFINITY);
        else
            _pDevice->set(iRow,RADIUS_CURVATURE,qsText.toDouble());
    }

    if (iCol==2) //"conic"
    {
        QTableWidgetItem* pItem=m_ui->twSurfacesDatas->item(iRow,iCol);
        QString qsText=pItem->text();

        double d=qsText.toDouble();
        _pDevice->set(iRow,CONIC,d);
    }

    if (iCol==3) //"z"
    {
        string sItem=m_ui->twSurfacesDatas->item(iRow,iCol)->text().toStdString();
        bool bAuto=sItem.find("auto")!=string::npos;
        if(bAuto)
            sItem=sItem.substr(sItem.find("auto")+4,string::npos);

        stringstream ss;
        ss << sItem;
        double dVal=0;
        ss >> dVal;

        _pDevice->set_z(iRow,dVal);
        _pDevice->set_autofocus(iRow,bAuto);
    }

    if (iCol==4) //"Diameter"
    {
        string sItem=m_ui->twSurfacesDatas->item(iRow,iCol)->text().toStdString();
        bool bAuto=sItem.find("auto")!=string::npos;
        if(bAuto)
            sItem=sItem.substr(sItem.find("auto")+4,string::npos);

        stringstream ss;
        ss << sItem;
        double dVal=0;
        ss >> dVal;

        _pDevice->set(iRow,DIAMETER,dVal);
        _pDevice->set(iRow,AUTO_DIAMETER,bAuto);
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

            stringstream ss;
            ss << sItem;
            double dVal;
            ss >> dVal;

            _pDevice->set(iRow,INNER_DIAMETER,dVal);
            _pDevice->set(iRow,AUTO_INNER_DIAMETER,bAuto);
        }

        iIndexCol++;
    }

    if(_bDisplayAspheric)
    {
        if (iCol==iIndexCol)
        {
            QTableWidgetItem* pItem=m_ui->twSurfacesDatas->item(iRow,iCol);
            _pDevice->set(iRow,R4,pItem->text().toDouble());
        }

        if (iCol==iIndexCol+1)
        {
            QTableWidgetItem* pItem=m_ui->twSurfacesDatas->item(iRow,iCol);
            _pDevice->set(iRow,R6,pItem->text().toDouble());
        }

        if (iCol==iIndexCol+2)
        {
            QTableWidgetItem* pItem=m_ui->twSurfacesDatas->item(iRow,iCol);
            _pDevice->set(iRow,R8,pItem->text().toDouble());
        }

        if (iCol==iIndexCol+3)
        {
            QTableWidgetItem* pItem=m_ui->twSurfacesDatas->item(iRow,iCol);
            _pDevice->set(iRow,R10,pItem->text().toDouble());
        }

        iIndexCol+=4;
    }

    if((iCol==iIndexCol) && _bDisplayComment)
    {
        QTableWidgetItem* pItem=m_ui->twSurfacesDatas->item(iRow,iCol);
        QString qsText=pItem->text();

        _pDevice->set_comment(iRow,qsText.toStdString());
        //    iCol++;
    }

    if (_bCanEmit==true)
        static_cast<MainWindow*>(parent())->update_views(this,PARAMETERS_CHANGED);
}
//////////////////////////////////////////////////////////////////////////////
void DockSurfacesData::OnAddSurfaceAfter()
{
    int iLine=m_ui->twSurfacesDatas->currentRow();
    if(iLine==-1)
    {
        if(_pDevice->nb_surface()!=0)
            return;
    }

    _pDevice->insert_surface(iLine+1);
    _pDevice->set_type(iLine+1,"void");

    if(iLine+1>0)
        _pDevice->set(iLine+1,AUTO_DIAMETER,true);

    //  device_changed(_pDevice);
    static_cast<MainWindow*>(parent())->update_views(this,NB_SURFACE_CHANGED);

    m_ui->twSurfacesDatas->selectRow(iLine+1);
}
//////////////////////////////////////////////////////////////////////////////
void DockSurfacesData::OnAddSurfaceBefore()
{
    int iLine=m_ui->twSurfacesDatas->currentRow();
    if(iLine==-1)
    {
        if(_pDevice->nb_surface()!=0)
            return;
        iLine=0;
    }

    _pDevice->insert_surface(iLine);
    _pDevice->set_type(iLine,"void");

    if(iLine>0)
        _pDevice->set(iLine,AUTO_DIAMETER,true);

    //  device_changed(_pDevice);
    static_cast<MainWindow*>(parent())->update_views(this,NB_SURFACE_CHANGED);

    m_ui->twSurfacesDatas->selectRow(iLine);
}
//////////////////////////////////////////////////////////////////////////////
void DockSurfacesData::OnDeleteSurface()
{
    int iLine=m_ui->twSurfacesDatas->currentRow();
    if(iLine==-1)
        return;

    _pDevice->delete_surface(iLine);
    //   device_changed(_pDevice);
    static_cast<MainWindow*>(parent())->update_views(this,NB_SURFACE_CHANGED);


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
    if(arg1=="Absolute")
        _pDevice->set_convention("absolute");

    if(arg1=="Relative")
        _pDevice->set_convention("relative");

    static_cast<MainWindow*>(parent())->update_views(this,PARAMETERS_CHANGED);
}
//////////////////////////////////////////////////////////////////////////////
