// this file is covered by the General Public License version 2 or later
// please see GPL.html for more details and licensing issues
// copyright Etienne de Foras ( the author )  mailto: etienne.deforas@gmail.com

#include "FrameSideView.h"
#include "MainWindow.h"
#include "ui_FrameSideView.h"
#include "AstreeDefines.h"
#include <QGraphicsScene>
#include <QGraphicsSceneWheelEvent>
#include <QGraphicsLineItem>

#include <cmath>
#include <cassert>
#include <iomanip>
#include <sstream>

#include "OpticalDevice.h"
#include "Light.h"
#include "Photon.h"
#include "GlassManager.h"
///////////////////////////////////////////////////////////////////////////////
class MySceneFrame : public QGraphicsScene
{
public:
    MySceneFrame(FrameSideView* pF)
    {
        _pF=pF;
    }

    virtual void wheelEvent(QGraphicsSceneWheelEvent* wheelEvent)
    {
        QGraphicsView* v=views()[0];
        v->setTransformationAnchor(QGraphicsView::AnchorUnderMouse);

        if (wheelEvent->delta()>0)
            v->scale(1.25,1.25);
        else
            v->scale(0.8,0.8);

        wheelEvent->accept();
    }

private:
    FrameSideView* _pF;
};
///////////////////////////////////////////////////////////////////////////////
FrameSideView::FrameSideView(QWidget *parent) :
    QFrame(parent),
    m_ui(new Ui::FrameSideView)
{
    _pDevice=0;

    m_ui->setupUi(this);
    m_ui->graphicsView->setScene(new MySceneFrame(this)); //TODO deleter la scene?
    m_ui->graphicsView->set_main_window((QMainWindow*)parent);
}

FrameSideView::~FrameSideView()
{
    delete m_ui;
}

void FrameSideView::changeEvent(QEvent *e)
{
    QFrame::changeEvent(e);
    switch (e->type())
    {
    case QEvent::LanguageChange:
        m_ui->retranslateUi(this);
        break;
    default:
        break;
    }
}
/////////////////////////////////////////////////////////////////////////////////////
void FrameSideView::device_changed(OpticalDevice* pDevice, int iReason)
{
    if( (iReason!=LIGHT_OFF_AXIS_CHANGED) && (iReason!=OPTICAL_DEVICE_CHANGED) && (iReason!=NEW_OPTICAL_DEVICE) )
        return;

    bool bSameDevice=(_pDevice==pDevice);
    _pDevice=pDevice;
    QGraphicsScene* scene=m_ui->graphicsView->scene();
    scene->clear();

    if (pDevice->nb_surface()==0)
        return;

    //draw each surface profile
    vector<double> viX1,viX2,viY1,viY2;
    int iLastSolidColor=0xffffff;
    for (int iSurf=0;iSurf<_pDevice->nb_surface();iSurf++)
    {
        double dZSurf=_pDevice->get(iSurf,Z);
        double dDiameter=_pDevice->get(iSurf,DIAMETER);
        double dMinDiameter=_pDevice->get(iSurf,INNER_DIAMETER);

        vector<double> viXNew1,viXNew2,viYNew1,viYNew2;
        for (int iStep=0;iStep<=SIDEVIEW_NB_POINTS_SURFACE;iStep++)
        {
            double dRadius=(dDiameter-dMinDiameter)/2.*iStep/(double)SIDEVIEW_NB_POINTS_SURFACE+dMinDiameter/2.;
            viXNew1.push_back(dRadius);
            viXNew2.push_back(-dRadius);
            double dZ;
            if(!_pDevice->compute_surface_profile(iSurf,0.,dRadius,dZ))
                return;

            assert(!isnan(dZ));
            viYNew1.push_back(dZSurf+dZ);
            viYNew2.push_back(dZSurf+dZ);
        }

        if(iLastSolidColor==0xffffff)
        {
            //disp as white so just a line
            for (unsigned int i=0;i<viXNew1.size()-1;i++)
            {
                QPen qp;
                qp.setCosmetic(true);
                //       qp.setWidth(2);

                scene->addLine(viYNew1[i],viXNew1[i],viYNew1[i+1],viXNew1[i+1],qp)->setZValue(10);
                scene->addLine(viYNew2[i],viXNew2[i],viYNew2[i+1],viXNew2[i+1],qp)->setZValue(10);
            }
        }
        else
        {
            //draw a polygon
            QPolygonF polygon1,polygon2;
            for (unsigned int i=0;i<viXNew1.size();i++)
            {
                polygon1 << QPointF(viYNew1[i],viXNew1[i]);
                polygon2 << QPointF(viYNew2[i],viXNew2[i]);
            }
            for (int i=(int)(viX1.size())-1;i>=0;i--)
            {
                polygon1 << QPointF(viY1[i],viX1[i]);
                polygon2 << QPointF(viY2[i],viX2[i]);
            }

            QPen qp;
            qp.setCosmetic(true);
            // qp.setWidth(2);

            scene->addPolygon(polygon1,qp,QBrush(QColor(QRgb(iLastSolidColor))))->setZValue(10);
            scene->addPolygon(polygon2,qp,QBrush(QColor(QRgb(iLastSolidColor))))->setZValue(10);
        }

        iLastSolidColor=GlassManager::singleton().solid_color(_pDevice->type(iSurf));

        viX1=viXNew1;
        viX2=viXNew2;
        viY1=viYNew1;
        viY2=viYNew2;
    }

    // plot ray fan
    Light light;

    double dPercentField=-1;
    if(!pDevice->get_parameter("showLightOffAxis",dPercentField))
        dPercentField=0.;

    assert(dPercentField>=0);
    assert(dPercentField<=100);
    double dTilt=dPercentField*pDevice->half_field_of_view()/100.;

    _pDevice->compute_light(&light,-1,dTilt,SIDEVIEW_NB_POINTS_LIGHT,1);
    vector<Photon> vp1=light.get_all_photons();
    for (int iS=0;iS<_pDevice->nb_surface();iS++)
    {
        _pDevice->compute_light(&light,iS,dTilt,SIDEVIEW_NB_POINTS_LIGHT,1); //TODO use ref
        vector<Photon> vp2=light.get_all_photons();
        for (unsigned int iP=0;iP<vp2.size();iP++)
        {
            const Photon& p1=vp1[iP];
            const Photon& p2=vp2[iP];

            if (p1.is_valid() && p2.is_valid())
            {
                QPen qpl;
                qpl.setColor(QColor(QRgb(light.get_visual_color(p1.lambda()))));
                qpl.setCosmetic(true);
                scene->addLine(p1.z,p1.x,p2.z,p2.x,qpl)->setZValue(20);
            }
        }

        vp1=vp2;
    }

    if (bSameDevice==false)
    {
        fit_in_view();
    }
    else
    {
        QRectF r=scene->sceneRect();
        QRectF r2=scene->itemsBoundingRect();
        enlarge(r2,1.1);
        scene->setSceneRect(r.united(r2));
    }
}
//////////////////////////////////////////////////////////////////////////////
void FrameSideView::enlarge(QRectF& r,double dRatio)
{
    double dX=r.width();
    double dY=r.height();
    QPointF c=r.center();
    r.setCoords( c.x()-dX*dRatio/2., c.y()-dY*dRatio/2., c.x()+dX*dRatio/2., c.y()+dY*dRatio/2.);
}
//////////////////////////////////////////////////////////////////////////////
void FrameSideView::fit_in_view()
{
    QGraphicsScene* scene=m_ui->graphicsView->scene();
    QRectF r=scene->itemsBoundingRect();
    enlarge(r,1.1);
    scene->setSceneRect(r);
    m_ui->graphicsView->fitInView(r,Qt::KeepAspectRatio);
}
//////////////////////////////////////////////////////////////////////////////
void FrameSideView::zoom_in()
{
    m_ui->graphicsView->scale(1.25,1.25);
}
//////////////////////////////////////////////////////////////////////////////
void FrameSideView::zoom_out()
{
    m_ui->graphicsView->scale(0.8,0.8);
}
//////////////////////////////////////////////////////////////////////////////
