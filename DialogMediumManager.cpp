#include "DialogMediumManager.h"
#include "ui_DialogMediumManager.h"

#include "GlassManager.h"
#include "Glass.h"

DialogMediumManager::DialogMediumManager(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogMediumManager)
{
    ui->setupUi(this);

    vector<string> lsM;
    GlassManager::singleton().list_available(lsM);

    ui->twMedium->setRowCount(lsM.size());

    QStringList qsl;
    qsl+="Maker";
    qsl+="Name";
    qsl+="Formulae";
    qsl+="N_Index";
    qsl+="V_Abbe";
    ui->twMedium->setColumnCount(qsl.size());
    ui->twMedium->setHorizontalHeaderLabels (qsl);

    for(unsigned int i=0;i<lsM.size();i++)
    {
        Glass* m=GlassManager::singleton().create(lsM[i]);

        QTableWidgetItem* qwmaker=new QTableWidgetItem(m->maker().c_str());
        ui->twMedium->setItem(i,0,qwmaker);

        QTableWidgetItem* qwname=new QTableWidgetItem(m->name().c_str());
        ui->twMedium->setItem(i,1,qwname);

        QTableWidgetItem* qwformula=new QTableWidgetItem(m->formula().c_str());
        ui->twMedium->setItem(i,2,qwformula);

        double dN,dV;
        m->compute_abbe_number(dN,dV);

        QTableWidgetItem* qwtiN=new QTableWidgetItem(QString::number(dN,'f',4));
        ui->twMedium->setItem(i,3,qwtiN);

        QTableWidgetItem* qwtiV=new QTableWidgetItem(QString::number(dV,'f',3));
        ui->twMedium->setItem(i,4,qwtiV);

        delete m;
    }
}

DialogMediumManager::~DialogMediumManager()
{
    delete ui;
}

void DialogMediumManager::on_pushButton_3_clicked()
{
    accept();
}
