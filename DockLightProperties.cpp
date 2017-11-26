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
    _bBlockSignals=true;

    m_ui->setupUi(this);

    connect(m_ui->leHalfFOV,SIGNAL(editingFinished()),this,SLOT(OnLightChange()));

    connect(m_ui->cbIR,SIGNAL(stateChanged(int)),this,SLOT(OnLightChange()));
    connect(m_ui->cbRed,SIGNAL(stateChanged(int)),this,SLOT(OnLightChange()));
    connect(m_ui->cbYellowBlack,SIGNAL(stateChanged(int)),this,SLOT(OnLightChange()));
    connect(m_ui->cbYellow,SIGNAL(stateChanged(int)),this,SLOT(OnLightChange()));
    connect(m_ui->cbGreen,SIGNAL(stateChanged(int)),this,SLOT(OnLightChange()));
    connect(m_ui->cbBlue,SIGNAL(stateChanged(int)),this,SLOT(OnLightChange()));
    connect(m_ui->cbUV,SIGNAL(stateChanged(int)),this,SLOT(OnLightChange()));

    _bBlockSignals=false;
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

void DockLightProperties::device_changed(OpticalDevice* pDevice,int iReason)
{
    if( (iReason!=OPTICAL_DEVICE_CHANGED) && (iReason!=NEW_OPTICAL_DEVICE) )
        return;

    _bBlockSignals=true;
    _pDevice=pDevice;

    double dHalfFOV=_pDevice->half_field_of_view();
    m_ui->leHalfFOV->setText(QString("%1").arg(dHalfFOV));

    int iNbStep=_pDevice->nb_intermediate_angles();
    m_ui->spNbAngles->setValue(iNbStep);

    string sLightColors=_pDevice->light_colors();

    m_ui->cbIR->setChecked((sLightColors.find("IR.")!=string::npos));
    m_ui->cbRed->setChecked((sLightColors.find("Red.")!=string::npos));
    m_ui->cbYellowBlack->setChecked((sLightColors.find("YellowBlack.")!=string::npos));
    m_ui->cbYellow->setChecked((sLightColors.find("Yellow.")!=string::npos));
    m_ui->cbGreen->setChecked((sLightColors.find("Green.")!=string::npos));
    m_ui->cbBlue->setChecked((sLightColors.find("Blue.")!=string::npos));
    m_ui->cbUV->setChecked((sLightColors.find("UV.")!=string::npos));

    _bBlockSignals=false;
}

void DockLightProperties::OnLightChange()
{  
    if(_bBlockSignals)
        return;

    double dHalfFOV=m_ui->leHalfFOV->text().toDouble();
    _pDevice->set_half_field_of_view(dHalfFOV);

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

    int iNbStep=m_ui->spNbAngles->value();
    if(iNbStep<1)
    {
        iNbStep=1;
       m_ui->spNbAngles->setValue(1);
    }
    _pDevice->set_nb_intermediate_angles(iNbStep);

    static_cast<MainWindow*>(parent())->device_changed(this,OPTICAL_DEVICE_CHANGED);
}


void DockLightProperties::on_spNbAngles_valueChanged(int arg1)
{
    (void)arg1;
    OnLightChange();
}
