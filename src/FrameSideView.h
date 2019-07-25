// this file is covered by the General Public License version 2 or later
// please see GPL.html for more details and licensing issues
// copyright Etienne de Foras ( the author )  mailto: etienne.deforas@gmail.com

#ifndef FRAMESIDEVIEW_H
#define FRAMESIDEVIEW_H

#include <QFrame>

#include <sstream>
#include <iomanip>
using namespace std;

namespace Ui
{
    class FrameSideView;
}

class OpticalDevice;
class FrameSideView : public QFrame
{
    Q_OBJECT
    Q_DISABLE_COPY(FrameSideView)
public:
    FrameSideView(QWidget *parent = 0);
    virtual ~FrameSideView();
    void device_changed(OpticalDevice *pDevice, int iReason);

    void fit_in_view();
    void zoom_in();
    void zoom_out();

protected:
    virtual void changeEvent(QEvent *e);

private:
    Ui::FrameSideView *m_ui;
    OpticalDevice* _pDevice;

    void enlarge(QRectF& r,double dRatio);
};

#endif // FRAMESIDEVIEW_H
