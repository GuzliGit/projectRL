#ifndef RL_SCENE_H
#define RL_SCENE_H

#include <QGraphicsScene>

class RL_scene : public QGraphicsScene
{
public:
    RL_scene(int width, int height, int scale_factor, QObject* parent = nullptr);
    void fill_with_empty_cells();
};

#endif // RL_SCENE_H
