// this file is covered by the General Public License version 2 or later
// please see GPL.html for more details and licensing issues
// copyright Etienne de Foras ( the author )  mailto: etienne.deforas@gmail.com


#include "DockLightProperties.h"
#include "ui_DockLightProperties.h"

#include "MainWindow.h"
#include "OpticalDevice.h"

DockLightProperties::DockLightProperties(QWidget *parent) :
    QDockWidget(parent),
    m_ui(new Ui::DockLightProperties)
{
    _bCanEmit=false;
    m_ui->setupUi(this);

    connect(m_ui->leHalfFOV,SIGNAL(editingFinished()),this,SLOT(OnLightChange()));
    connect(m_ui->leNbSteps,SIGNAL(editingFinished()),this,SLOT(OnLightChange()));

    connect(m_ui->cbIR,SIGNAL(stateChanged(int)),this,SLOT(OnLightChange()));
    connect(m_ui->cbRed,SIGNAL(stateChanged(int)),this,SLOT(OnLightChange()));
    connect(m_ui->cbYellowBlack,SIGNAL(stateChanged(int)),this,SLOT(OnLightChange()));
    connect(m_ui->cbYellow,SIGNAL(stateChanged(int)),this,SLOT(OnLightChange()));
    connect(m_ui->cbGreen,SIGNAL(stateChanged(int)),this,SLOT(OnLightChange()));
    connect(m_ui->cbBlue,SIGNAL(stateChanged(int)),this,SLOT(OnLightChange()));
    connect(m_ui->cbUV,SIGNAL(stateChanged(int)),this,SLOT(OnLightChange()));
}

DockLightProperties::~DockLightProperties()
{
    delete m_ui;
}

void DockLightProperties::changeEvent(QEvent *e)
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

void DockLightProperties::device_changed(OpticalDevice* pDevice)
{
    //TODO update in case if device change /load

    _bCanEmit=false;
    _pDevice=pDevice;
    //double dTiltX=0.,dTiltY=0.;

    double dHalfFOV=_pDevice->half_field_of_view();
    m_ui->leHalfFOV->setText(QString("%1").arg(dHalfFOV));

    int iNbStep=_pDevice->nb_intermediate_angles();
    m_ui->leNbSteps->setText(QString("%1").arg(iNbStep));

    string sLightColors=_pDevice->light_colors();

    m_ui->cbIR->setCheckState((sLightColors.find("IR.")!=string::npos)?Qt::Checked:Qt::Unchecked);
    m_ui->cbRed->setCheckState((sLightColors.find("Red.")!=string::npos)?Qt::Checked:Qt::Unchecked);
    m_ui->cbYellowBlack->setCheckState((sLightColors.find("YellowBlack.")!=string::npos)?Qt::Checked:Qt::Unchecked);
    m_ui->cbYellow->setCheckState((sLightColors.find("Yellow.")!=string::npos)?Qt::Checked:Qt::Unchecked);
    m_ui->cbGreen->setCheckState((sLightColors.find("Green.")!=string::npos)?Qt::Checked:Qt::Unchecked);
    m_ui->cbBlue->setCheckState((sLightColors.find("Blue.")!=string::npos)?Qt::Checked:Qt::Unchecked);
    m_ui->cbUV->setCheckState((sLightColors.find("UV.")!=string::npos)?Qt::Checked:Qt::Unchecked);

    _bCanEmit=true;
}

void DockLightProperties::OnLightChange()
{  
    double dHalfFOV=m_ui->leHalfFOV->text().toDouble();
    _pDevice->set_half_field_of_view(dHalfFOV);

    //    _pDevice->set_light_tilt((double)m_ui->hsImageFieldPos->value()/m_ui->hsImageFieldPos->maximum()*_pDevice->get_half_field_of_view());
    //   ((MainWindow*)parent())->dLightTilt=(double)m_ui->hsImageFieldPos->value()/m_ui->hsImageFieldPos->maximum()*_pDevice->get_half_field_of_view();

    string sLightColors;

    if (m_ui->cbIR->checkState ()==Qt::Checked)
        sLightColors+="IR.";

    if (m_ui->cbRed->checkState ()==Qt::Checked)
        sLightColors+="Red.";

    if (m_ui->cbYellowBlack->checkState ()==Qt::Checked)
        sLightColors+="YellowBlack.";

    if (m_ui->cbYellow->checkState()==Qt::Checked)
        sLightColors+="Yellow.";

    if (m_ui->cbGreen->checkState()==Qt::Checked)
        sLightColors+="Green.";

    if (m_ui->cbBlue->checkState()==Qt::Checked)
        sLightColors+="Blue.";

    if (m_ui->cbUV->checkState()==Qt::Checked)
        sLightColors+="UV.";

    _pDevice->set_light_colors(sLightColors);

    int iNbStep=m_ui->leNbSteps->text().toInt();
    if(iNbStep<3)
    {
        iNbStep=3;
        m_ui->leNbSteps->setText("3");
    }
    _pDevice->set_nb_intermediate_angles(iNbStep);

    if(_bCanEmit==true)
        static_cast<MainWindow*>(parent())->update_views(this);
}
