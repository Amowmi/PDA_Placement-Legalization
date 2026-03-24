#pragma once
#include <vector>

class Cell;                  // <── forward declaration; full definition not needed

class Cluster {
public:
    double  x   = 0.0;
    int     width  = 0;
    int     weight = 0;      // e in the slides
    double  q   = 0.0;       // Σ w·(x−width)

    Cluster* prev_cluster = nullptr;
    std::vector<Cell*> cell_inside;

    Cluster() = default;
    Cluster(double x, Cluster* prev) : x(x), prev_cluster(prev) {}

    void addCell(Cell* c);   // definition lives in Cluster.cpp
};
