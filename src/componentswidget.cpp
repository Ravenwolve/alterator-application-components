#include "componentswidget.h"

namespace amc
{
ComponentsWidget::ComponentsWidget(QWidget *parent)
    : ui(new Ui::ComponentsWidget)
{
    ui->setupUi(this);
    ui->componentsTreeView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->componentsTreeView->setHeaderHidden(true);
}

ComponentsWidget::~ComponentsWidget()
{
    delete ui;
}

void ComponentsWidget::setController(ControllerInterface *controller)
{
    m_controller = controller;
}

void ComponentsWidget::setComponentsModel(amc::model::Model *model)
{
    ui->componentsTreeView->setModel(model);

    connect(ui->componentsTreeView->selectionModel(),
            &QItemSelectionModel::selectionChanged,
            this,
            &ComponentsWidget::onSelectionChanged);
}

void ComponentsWidget::setDescription(const QString &description)
{
    ui->descriptionTextEdit->setText(description);
}

void ComponentsWidget::onSelectionChanged(const QItemSelection &newSelection, const QItemSelection &previousSelection)
{
    if (newSelection.isEmpty())
    {
        return;
    }

    QModelIndex index = newSelection.indexes().at(0);

    m_controller->ComponentSelected(index);
}
} // namespace amc
