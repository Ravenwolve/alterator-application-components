#ifndef WAITDIALOG_H
#define WAITDIALOG_H

#include <QDialog>

namespace Ui
{
class WaitDialog;
}

class WaitDialog : public QDialog
{
    Q_OBJECT

public:
    explicit WaitDialog(QWidget *parent = nullptr);
    ~WaitDialog() override;

    void clearUi();
    void addLogLine(QString line, int result);
    void beginApply(const int installPackagesCount, const int removePackageCount);
    void endApply();
    void beginInstallPackage(QString package);
    void endInstallPackage(QString package, int result);
    void beginRemovePackage(QString package);
    void endRemovePackage(QString package, int result);

private slots:
    void on_okButtonclicked();

private:
    void clearState();
    void beginAnimation();
    void stopAnimation();
    void setWaitCursor();
    void setNormalCursor();

private:
    Ui::WaitDialog *ui;
    bool errorFlag;
};

#endif // WAITDIALOG_H
