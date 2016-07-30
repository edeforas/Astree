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
    qsl+="Nd";
    qsl+="Vd";
    qsl+="Ne";
    qsl+="Ve";

    ui->twMedium->setColumnCount(qsl.size());
    ui->twMedium->setHorizontalHeaderLabels (qsl);

    for(int i=0;i<(int)lsM.size();i++)
    {
        Glass* m=GlassManager::singleton().create(lsM[i]);

        QTableWidgetItem* qwmaker=new QTableWidgetItem(m->maker().c_str());
        ui->twMedium->setItem(i,0,qwmaker);

        QTableWidgetItem* qwname=new QTableWidgetItem(m->name().c_str());
        ui->twMedium->setItem(i,1,qwname);

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
}

DialogMediumManager::~DialogMediumManager()
{
    delete ui;
}

void DialogMediumManager::on_pushButton_3_clicked()
{
    accept();
}
