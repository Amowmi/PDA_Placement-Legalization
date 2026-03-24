#include "Cluster.hpp"
#include "Cell.hpp"          // full type needed only in the .cpp

void Cluster::addCell(Cell* c)
{
    cell_inside.push_back(c);
    weight += c->weight;
    q     += c->weight * (c->x - width);
    width += c->w;
}
