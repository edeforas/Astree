// this file is covered by the  GNU LESSER GENERAL PUBLIC LICENSE Version 3 or later
// please see LICENSE.txt for more details and licensing issues
// copyright Etienne de Foras ( the author )  mailto: etienne.deforas@gmail.com

#include "DialogMediumManager.h"
#include "ui_DialogMediumManager.h"

#include "GlassManager.h"
#include "Glass.h"

//////////////////////////////////////////////////////////////////////
DialogMediumManager::DialogMediumManager(QWidget *parent, const string& sGlass) :
    QDialog(parent),
    ui(new Ui::DialogMediumManager)
{
    ui->setupUi(this);

	// init glass tab
    vector<string> vsGlass;
    GlassManager::singleton().list_available(vsGlass);

    ui->twMedium->setRowCount((int)vsGlass.size());

    QStringList qsl;
    qsl+="Maker";
    qsl+="Name";
    qsl+="Formulae";
    qsl+="Nd";
    qsl+="Vd";
    qsl+="Ne";
    qsl+="Ve";

    ui->twMedium->setColumnCount(qsl.size());
    ui->twMedium->setHorizontalHeaderLabels (qsl);

    int iPosGlass=-1;

    for(int i=0;i<(int)vsGlass.size();i++)
    {
        Glass* m=GlassManager::singleton().create(vsGlass[i]);

        QTableWidgetItem* qwmaker=new QTableWidgetItem(m->maker().c_str());
        ui->twMedium->setItem(i,0,qwmaker);

        QTableWidgetItem* qwname=new QTableWidgetItem(m->name().c_str());
        ui->twMedium->setItem(i,1,qwname);

        if(sGlass==m->name())
            iPosGlass=i;

        QTableWidgetItem* qwformula=new QTableWidgetItem(m->formula().c_str());
        ui->twMedium->setItem(i,2,qwformula);

        double dNd,dVd,dNe,dVe;
        m->compute_NdVd(dNd,dVd);
        m->compute_NeVe(dNe,dVe);

        QTableWidgetItem* qwtiNd=new QTableWidgetItem(QString::number(dNd,'f',5));
        ui->twMedium->setItem(i,3,qwtiNd);

        QTableWidgetItem* qwtiVd=new QTableWidgetItem(QString::number(dVd,'f',3));
        ui->twMedium->setItem(i,4,qwtiVd);

        QTableWidgetItem* qwtiNe=new QTableWidgetItem(QString::number(dNe,'f',5));
        ui->twMedium->setItem(i,5,qwtiNe);

        QTableWidgetItem* qwtiVe=new QTableWidgetItem(QString::number(dVe,'f',3));
        ui->twMedium->setItem(i,6,qwtiVe);

        delete m;
    }

    if(iPosGlass!=-1)
    {
        ui->twMedium->setCurrentCell(iPosGlass,1);
        ui->btnOk->hide();
    }
    else
    {
        ui->btnSelect->hide();
        ui->btnCancel->hide();
    }

	// init catalogs tab
	vector<string> vsCatalogs;
	GlassManager::singleton().list_catalogs(vsCatalogs);
	ui->twCatalogs->setRowCount((int)vsCatalogs.size());
	ui->twCatalogs->setColumnCount(1);
	QStringList qsc;
	qsc += "Maker";
	ui->twCatalogs->setHorizontalHeaderLabels(qsc);
	for (int i = 0; i < (int)vsCatalogs.size(); i++)
	{
		QTableWidgetItem* qwmaker = new QTableWidgetItem(vsCatalogs[i].c_str());
		ui->twCatalogs->setItem(i, 0, qwmaker);
	}
}
//////////////////////////////////////////////////////////////////////
DialogMediumManager::~DialogMediumManager()
{
    delete ui;
}
//////////////////////////////////////////////////////////////////////
string DialogMediumManager::selected_glass() const
{
    return _sSelectedGlass;
}
//////////////////////////////////////////////////////////////////////
void DialogMediumManager::on_btnSelect_clicked()
{
    int iRow=ui->twMedium->currentRow();
    _sSelectedGlass=ui->twMedium->item(iRow,1)->text().toStdString();

    accept();
}
//////////////////////////////////////////////////////////////////////
void DialogMediumManager::on_btnCancel_clicked()
{
    reject();
}
//////////////////////////////////////////////////////////////////////
void DialogMediumManager::on_btnOk_clicked()
{
    _sSelectedGlass="";
    accept();
}
//////////////////////////////////////////////////////////////////////
