#ifndef MINIFLOWLAYOUT_H
#define MINIFLOWLAYOUT_H

#include <QGridLayout>

class MiniFlowLayout : public QGridLayout
{
    Q_OBJECT
public:
    MiniFlowLayout(QWidget *parent = nullptr);
    void add_widget(QWidget *item);
    void rearrange_widgets(int width);

private:
    QList<QWidget*> current_items;
    int current_cols;
};

#endif // MINIFLOWLAYOUT_H
