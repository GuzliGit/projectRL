#include "miniflowlayout.h"
#include <QGridLayout>
#include <QWidget>
#include <QtGlobal>

MiniFlowLayout::MiniFlowLayout(QWidget *parent) : QGridLayout(parent)
{
    setSpacing(10);
    setContentsMargins(5, 5, 5, 5);
}

void MiniFlowLayout::add_widget(QWidget *item)
{
    current_items.append(item);
    rearrange_widgets(parentWidget()->size().width());
}

void MiniFlowLayout::rearrange_widgets(int width)
{
    while (count() > 0)
        takeAt(0);

    if (current_items.isEmpty())
        return;

    int item_width = current_items.first()->sizeHint().width() + spacing() + contentsMargins().left() + contentsMargins().right();
    current_cols = qMax(1, width / item_width);

    for (int i = 0; i < current_items.size(); ++i)
    {
        int row = i / current_cols;
        int col = i % current_cols;
        QGridLayout::addWidget(current_items[i], row, col);
    }
}
