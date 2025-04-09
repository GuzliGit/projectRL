#ifndef WIDGETWITHFLOWLAYOUT_H
#define WIDGETWITHFLOWLAYOUT_H

#include "miniflowlayout.h"
#include <QWidget>

class WidgetWithFlowLayout : public QWidget
{
    Q_OBJECT
public:
    WidgetWithFlowLayout(QWidget *parent = nullptr);
    void add_to_layout(QWidget *item);

protected:
    void resizeEvent(QResizeEvent *event) override;

private:
    MiniFlowLayout *flow_layout;
};

#endif // WIDGETWITHFLOWLAYOUT_H
