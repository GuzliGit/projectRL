#include "rl_scene.h"
#include "cellitem.h"

RL_scene::RL_scene(int width, int height, int scale_factor, QObject* parent) : QGraphicsScene(parent)
{
    setSceneRect(0, 0, width * scale_factor, height * scale_factor);
}

void RL_scene::fill_with_empty_cells()
{
    CellItem *temp = new CellItem();
    int cell_width = temp->get_width();
    int cell_height = temp->get_height();
    int cols = this->width() / cell_width;
    int rows = this->height() / cell_height;

    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            CellItem *cell = new CellItem();
            cell->setPos(j * cell_width, i * cell_height);
            this->addItem(cell);
        }
    }
}
