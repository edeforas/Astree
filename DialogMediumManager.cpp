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
    ui->twMedium->setColumnCount(4);

    QStringList qsl;
    qsl+="Name";
    qsl+="Formulae";
    qsl+="N_Index";
    qsl+="V_Abbe";
    ui->twMedium->setHorizontalHeaderLabels (qsl);

    for(unsigned int i=0;i<lsM.size();i++)
    {
        Glass* m=GlassManager::singleton().create(lsM[i]);

        QTableWidgetItem* qwti=new QTableWidgetItem(m->name().c_str());
        ui->twMedium->setItem(i,0,qwti);

        QTableWidgetItem* qwtif=new QTableWidgetItem(m->formulae().c_str());
        ui->twMedium->setItem(i,1,qwtif);

        double dN,dV;
        m->compute_abbe_number(dN,dV);

        QTableWidgetItem* qwtiN=new QTableWidgetItem(QString::number(dN,'f',4));
        ui->twMedium->setItem(i,2,qwtiN);

        QTableWidgetItem* qwtiV=new QTableWidgetItem(QString::number(dV,'f',3));
        ui->twMedium->setItem(i,3,qwtiV);

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
