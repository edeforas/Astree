#include "DialogRevertDesign.h"
#include "ui_DialogRevertDesign.h"

DialogRevertDesign::DialogRevertDesign(int iNbSurfaces,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogRevertDesign)
{
    ui->setupUi(this);

	ui->spFirstSurface->setRange(1, iNbSurfaces + 1);
	ui->spLastSurface->setRange(1, iNbSurfaces + 1);
	ui->spFirstSurface->setValue( 1);
	ui->spLastSurface->setValue(iNbSurfaces + 1);
}

DialogRevertDesign::~DialogRevertDesign()
{
    delete ui;
}

int DialogRevertDesign::first_surface()
{
	if(ui->spFirstSurface->value()<= ui->spLastSurface->value())
		return ui->spFirstSurface->value()-1;
	else
		return ui->spLastSurface->value() - 1;
}

int DialogRevertDesign::last_surface()
{
	if (ui->spFirstSurface->value() <= ui->spLastSurface->value())
		return ui->spLastSurface->value() - 1;
	else
		return ui->spFirstSurface->value() - 1;
}

void DialogRevertDesign::on_btnOk_clicked()
{
	accept();
}

void DialogRevertDesign::on_btnCancel_clicked()
{
	reject();
}
