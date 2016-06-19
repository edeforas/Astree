#include "DockCommentary.h"
#include "ui_DockCommentary.h"

#include "MainWindow.h"
#include "OpticalDevice.h"

#include <cassert>

DockCommentary::DockCommentary(QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::DockComment)
{
    ui->setupUi(this);
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
    assert(pDevice!=0);
    _pDevice=pDevice;

    if(iReason!=NEW_OPTICAL_DEVICE)
        return;

    ui->textEdit->blockSignals(true);
    ui->textEdit->setText(_pDevice->note().c_str());
    ui->textEdit->blockSignals(false);
}
////////////////////////////////////////////////////////////////////////
void DockCommentary::on_textEdit_textChanged()
{
    if(_pDevice==0)
        return;

    _pDevice->set_note(ui->textEdit->document()->toPlainText().toStdString());
    static_cast<MainWindow*>(parent())->update_views(this,COMMENT_CHANGED);
}
////////////////////////////////////////////////////////////////////////
