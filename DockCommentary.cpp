#include "DockCommentary.h"
#include "ui_DockCommentary.h"

#include "MainWindow.h"

#include "OpticalDevice.h"

DockCommentary::DockCommentary(QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::DockComment)
{
    _bCanEmit=false;
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

void DockCommentary::device_changed(OpticalDevice* pDevice)
{
    _bCanEmit=false;
    _pDevice=pDevice;
    if(_pDevice!=0)
    {
        string sComment;
        sComment=_pDevice->note();
        ui->textEdit->setText(sComment.c_str());
    }
    else
        ui->textEdit->clear();
    _bCanEmit=true;
}


void DockCommentary::on_textEdit_textChanged()
{
    if(_pDevice!=0)
    {
        if(_bCanEmit)
        {
            _pDevice->set_note(ui->textEdit->document()->toPlainText().toStdString());
            static_cast<MainWindow*>(parent())->update_views(this); //uncesserary repaint
        }
    }
}
