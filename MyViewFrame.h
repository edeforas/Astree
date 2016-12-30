// this file is covered by the General Public License version 2 or later
// please see GPL.html for more details and licensing issues
// copyright Etienne de Foras ( the author )  mailto: etienne.deforas@gmail.com

#ifndef MyViewFrame_
#define MyViewFrame_

#include <QGraphicsView>
#include <QMainWindow>
#include <QMouseEvent>
#include <QStatusBar>

class MyViewFrame : public QGraphicsView
{
public:
    MyViewFrame(QWidget *parent = 0):QGraphicsView(parent)
    {
        _pMW=0;
        setMouseTracking (true);
        setCursor(Qt::CrossCursor);
        setRenderHints( QPainter::Antialiasing );
    }

    void mousePressEvent ( QMouseEvent * event )
    {
        setDragMode(ScrollHandDrag);
        QGraphicsView::mousePressEvent(event);
    }

    void mouseReleaseEvent ( QMouseEvent * event )
    {
        setDragMode(NoDrag );
        QGraphicsView::mouseReleaseEvent(event);
        setCursor(Qt::CrossCursor);
    }

    void set_main_window(QMainWindow* pMW)
    { _pMW=pMW; }

    void mouseMoveEvent ( QMouseEvent * event )
    {
        if(_pMW==0)
            return;

        QPointF pf=mapToScene(event->x(),event->y());
        stringstream ss;
        ss << setprecision(5) << "Mouse Position(mm): x=" << 0 << " y=" << -pf.y() << " z=" << pf.x();

        QStatusBar* ps=_pMW->statusBar();
        ps->showMessage(ss.str().c_str());

        QGraphicsView::mouseMoveEvent(event);
    }

private:
    QMainWindow* _pMW;
};

#endif
