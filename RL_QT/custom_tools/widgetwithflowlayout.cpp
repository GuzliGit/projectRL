#include "widgetwithflowlayout.h"
#include <QResizeEvent>
WidgetWithFlowLayout::WidgetWithFlowLayout(QWidget *parent) : QWidget(parent)
{
    flow_layout = new MiniFlowLayout(this);
}

void WidgetWithFlowLayout::add_to_layout(QWidget *item)
{
    flow_layout->add_widget(item);
}

void WidgetWithFlowLayout::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);

    flow_layout->rearrange_widgets(event->size().width());
}
