#pragma once
#include <cmath>
#include <limits>

class Subrow;  // forward declaration

class Cell {
   public:
    const int idx;
    const int w;
    const int h;

    const double x = 0.0, y = 0.0;  // global coordinates
    double legal_x = 0.0, legal_y = 0.0;
    int weight;  // default weight
    int row_id = -1;
    Subrow* subrow = nullptr;

    Cell() = default;
    Cell(int idx, int w, int h, double x, double y)
        : idx(idx), w(w), h(h), x(x), y(y) {
        weight = 1;
    }

    void assign(int new_x, int new_y) {
        legal_x = new_x;
        legal_y = new_y;
    }

    double getDistance() const {
        if (x == std::numeric_limits<double>::max() ||
            y == std::numeric_limits<double>::max())
            return std::numeric_limits<double>::max();

        double dx = x - legal_x;
        double dy = y - legal_y;
        return std::sqrt(dx * dx + dy * dy);
    }
};
