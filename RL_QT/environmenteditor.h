#ifndef ENVIRONMENTEDITOR_H
#define ENVIRONMENTEDITOR_H

#include "cellitem.h"

class EnvironmentEditor
{
public:
    EnvironmentEditor();
    void change_cells(QList<CellItem*>& selected_cells, CellType type);
};

#endif // ENVIRONMENTEDITOR_H
