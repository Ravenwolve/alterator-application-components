#include "packagesdialog.h"
#include "ui_packagesdialog.h"

PackagesDialog::PackagesDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::PackagesDialog)
{
    ui->setupUi(this);
}

PackagesDialog::~PackagesDialog()
{
    delete ui;
}

void PackagesDialog::setInstallPackages(const QMap<QString, QString> installPackages)
{
    for (QString package : installPackages.keys())
    {
        ui->installTextEdit->append(package);
    }
}

void PackagesDialog::setRemovePackages(const QMap<QString, QString> removePackages)
{
    for (QString package : removePackages.keys())
    {
        ui->removeTextEdit->append(package);
    }
}
