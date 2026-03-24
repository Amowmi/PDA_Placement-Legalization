#pragma once
#include <iostream>
#include <string>
#include <vector>

#include "Cell.hpp"
#include "Cluster.hpp"

using namespace std;

class Subrow {
   public:
    int h;
    int min_x, max_x;
    int y;
    int unoccupied;
    int site_width;
    Cluster* last_cluster;

    Subrow() = default;
    Subrow(int h, int min_x, int max_x, int y, int site_width)
        : h(h), min_x(min_x), max_x(max_x), y(y), site_width(site_width){
        unoccupied = max_x - min_x;
        last_cluster = nullptr;
    }

    void update(int new_l, int new_r) {
        min_x = new_l;
        max_x = new_r;
    }
};
