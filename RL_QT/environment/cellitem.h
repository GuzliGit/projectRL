#ifndef CELLITEM_H
#define CELLITEM_H

#include <QGraphicsPixmapItem>
#include <QPropertyAnimation>
#include <QParallelAnimationGroup>

enum class CellType
{
    Empty,
    Floor,
    Wall
};

class CellItem : public QObject, public QGraphicsPixmapItem
{
    Q_OBJECT
    Q_PROPERTY(qreal opacity READ opacity WRITE setOpacity)  // Прозрачность
    Q_PROPERTY(qreal scale READ scale WRITE setScale)        // Масштаб

public:
    CellItem(QGraphicsItem *parent = nullptr);
    int get_width();
    int get_height();
    void set_selected(bool cell_selected, bool ctrl_pressed);
    bool is_selected() const;
    void allow_selection();
    bool selection_was_changed() const;
    void reset_animation();
    virtual CellType get_type() const { return CellType::Empty; }
    virtual void update_cell_appearance();
    virtual bool is_walkable() const { return false; };

private:
    int width;
    int height;
    bool selected = false;
    bool current_selection_changed = false;
    QParallelAnimationGroup *selection_animation;

    void animate_selection();
    void stop_animation();
};

#endif // CELLITEM_H
