#include "DialogScaleDevice.h"
#include "ui_DialogScaleDevice.h"

#include <QMessageBox>

DialogScaleDevice::DialogScaleDevice(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogScaleDevice)
{
    ui->setupUi(this);

    _dScale=-1.;
    _bScaleDiameter=false;
    _bScaleFocal=false;
}

DialogScaleDevice::~DialogScaleDevice()
{
    delete ui;
}

void DialogScaleDevice::on_pushButton_clicked()
{
    bool bOk=false;

    if(ui->rbFactor->isChecked())
    {
        _dScale=ui->leFactor->text().toDouble(&bOk);
        if(!bOk||(_dScale<=0.))
        {
            QMessageBox::warning(this,"Error","Please, enter a valid ratio");
            return;
        }
    }

    if(ui->rbPercentage->isChecked())
    {
        _dScale=ui->lePercentage->text().toDouble(&bOk)/100.;
        if(!bOk||(_dScale<=0.))
        {
            QMessageBox::warning(this,"Error","Please, enter a valid percentage");
            return;
        }
    }

    if(ui->rbSizeChange->isChecked())
    {
        double dOldSize=ui->leOldSize->text().toDouble(&bOk);
        if(!bOk||(dOldSize<=0.))
        {
            QMessageBox::warning(this,"Error","Please, enter a valid old size");
            return;
        }
        double dNewSize=ui->leNewSize->text().toDouble(&bOk);
        if(!bOk|| (dNewSize<=0.))
        {
            QMessageBox::warning(this,"Error","Please, enter a valid old size");
            return;
        }

        _dScale=dNewSize/dOldSize;
    }

    _bScaleDiameter=ui->cbScaleDiameter->isChecked();
    _bScaleFocal=ui->cbScaleFocal->isChecked();

    accept();
}

void DialogScaleDevice::on_pushButton_2_clicked()
{
    reject();
}

void DialogScaleDevice::on_rbFactor_clicked()
{
    ui->leFactor->setEnabled(true);
    ui->lePercentage->setEnabled(false);
    ui->leOldSize->setEnabled(false);
    ui->leNewSize->setEnabled(false);
}

void DialogScaleDevice::on_rbPercentage_clicked()
{
    ui->leFactor->setEnabled(false);
    ui->lePercentage->setEnabled(true);
    ui->leOldSize->setEnabled(false);
    ui->leNewSize->setEnabled(false);
}

void DialogScaleDevice::on_rbSizeChange_clicked()
{
    ui->leFactor->setEnabled(false);
    ui->lePercentage->setEnabled(false);
    ui->leOldSize->setEnabled(true);
    ui->leNewSize->setEnabled(true);
}

double DialogScaleDevice::get_scale()
{
    return _dScale;
}

bool DialogScaleDevice::get_scale_diameter()
{
    return _bScaleDiameter;
}

bool DialogScaleDevice::get_scale_focal()
{
    return _bScaleFocal;
}
