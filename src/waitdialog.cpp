#include "waitdialog.h"
#include "ui_waitdialog.h"

#include "QPushButton"

WaitDialog::WaitDialog(QWidget *parent)
    : QDialog(parent, Qt::FramelessWindowHint)
    , ui(new Ui::WaitDialog)
    , errorFlag(false)
{
    ui->setupUi(this);
    connect(ui->buttonBox->button(QDialogButtonBox::StandardButton::Ok),
            &QPushButton::clicked,
            this,
            &WaitDialog::on_okButtonclicked);
}

WaitDialog::~WaitDialog()
{
    delete ui;
}

void WaitDialog::clearUi() {}

void WaitDialog::addLogLine(QString line, int result)
{
    if (result)
    {
        QTextCharFormat fmt = ui->logTextEdit->currentCharFormat();
        fmt.setForeground(QBrush(Qt::red));
        ui->logTextEdit->setCurrentCharFormat(fmt);

        ui->logTextEdit->append(line);

        fmt = ui->logTextEdit->currentCharFormat();
        fmt.clearForeground();
        ui->logTextEdit->setCurrentCharFormat(fmt);
    }
    else
    {
        ui->logTextEdit->append(line);
    }
}

void WaitDialog::beginApply(const int installPackagesCount, const int removePackageCount)
{
    setWaitCursor();

    clearState();

    auto okButton = ui->buttonBox->button(QDialogButtonBox::StandardButton::Ok);

    okButton->setEnabled(false);
}

void WaitDialog::endApply()
{
    setNormalCursor();

    if (errorFlag)
    {
        addLogLine(QString(tr("Some errors occured.")), 1);
    }
    else
    {
        addLogLine(QString(tr("Changes applied.")), 0);
    }

    auto okButton = ui->buttonBox->button(QDialogButtonBox::StandardButton::Ok);

    okButton->setEnabled(true);
}

void WaitDialog::beginInstallPackage(QString package)
{
    addLogLine(QString(tr("Installing packages: ") + package), 0);
}

void WaitDialog::endInstallPackage(QString package, int result)
{
    if (!result)
    {
        addLogLine(QString(tr("Packaged installed successfully.\n")), result);
    }
    else
    {
        addLogLine(QString(tr("Could not install packages: ") + package), result);
        errorFlag = true;
    }
}

void WaitDialog::beginRemovePackage(QString package)
{
    addLogLine(QString(tr("Removing packages: ") + package), 0);
}

void WaitDialog::endRemovePackage(QString package, int result)
{
    if (!result)
    {
        addLogLine(QString(tr("Packaged removed successfully.\n")), result);
    }
    else
    {
        addLogLine(QString(tr("Could not remove packages: ") + package), result);
        errorFlag = true;
    }
}

void WaitDialog::on_okButtonclicked()
{
    this->close();
}

void WaitDialog::clearState()
{
    errorFlag = false;

    ui->logTextEdit->clear();
}

void WaitDialog::beginAnimation() {}

void WaitDialog::stopAnimation() {}

void WaitDialog::setWaitCursor()
{
    setCursor(Qt::WaitCursor);
    ui->logTextEdit->viewport()->setCursor(Qt::WaitCursor);
}

void WaitDialog::setNormalCursor()
{
    setCursor(Qt::ArrowCursor);
    ui->logTextEdit->viewport()->setCursor(Qt::ArrowCursor);
}
