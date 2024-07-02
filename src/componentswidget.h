#ifndef COMPONENTSWIDGET_H
#define COMPONENTSWIDGET_H

#include "controllerinterface.h"
#include "model/model.h"
#include "ui_componentswidget.h"

namespace amc
{
class ComponentsWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ComponentsWidget(QWidget *parent = nullptr);
    ~ComponentsWidget() override;

    void setController(ControllerInterface *controller);
    void setComponentsModel(amc::model::Model *model);
    void setDescription(const QString &description);

public:
    ComponentsWidget(const ComponentsWidget &)            = delete;
    ComponentsWidget(ComponentsWidget &&)                 = delete;
    ComponentsWidget &operator=(const ComponentsWidget &) = delete;
    ComponentsWidget &operator=(ComponentsWidget &&)      = delete;

private slots:
    void onSelectionChanged(const QItemSelection &newSelection, const QItemSelection &previousSelection);

private:
    Ui::ComponentsWidget *ui;
    ControllerInterface *m_controller;
};

} // namespace amc

#endif // COMPONENTSWIDGET_H
