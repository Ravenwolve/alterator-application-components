#ifndef PACKAGESDIALOG_H
#define PACKAGESDIALOG_H

#include <QDialog>

namespace Ui
{
class PackagesDialog;
}

class PackagesDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PackagesDialog(QWidget *parent = nullptr);
    ~PackagesDialog() override;

    void setInstallPackages(const QMap<QString, QString> installPackages);
    void setRemovePackages(const QMap<QString, QString> removePackages);

private:
    Ui::PackagesDialog *ui;
};

#endif // PACKAGESDIALOG_H
