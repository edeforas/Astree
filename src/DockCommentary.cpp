#include "DockCommentary.h"
#include "ui_DockCommentary.h"

#include "MainWindow.h"
#include "OpticalDevice.h"

#include <cassert>

DockCommentary::DockCommentary(QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::DockComment)
{
    _bBlockSignals=true;
    ui->setupUi(this);
    _bBlockSignals=false;
}

DockCommentary::~DockCommentary()
{
    delete ui;
}

void DockCommentary::changeEvent(QEvent *e)
{
    QDockWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}
////////////////////////////////////////////////////////////////////////
void DockCommentary::device_changed(OpticalDevice* pDevice,int iReason)
{
    if(iReason!=NEW_OPTICAL_DEVICE)
        return;

    assert(pDevice!=0);
    _pDevice=pDevice;

    _bBlockSignals=true;
    ui->textEdit->setText(_pDevice->note().c_str());
    _bBlockSignals=false;
}
////////////////////////////////////////////////////////////////////////
void DockCommentary::on_textEdit_textChanged()
{   
    if(_bBlockSignals)
        return;

    _pDevice->set_note(ui->textEdit->document()->toPlainText().toStdString());
    static_cast<MainWindow*>(parent())->device_changed(this,COMMENT_CHANGED);
}
////////////////////////////////////////////////////////////////////////
