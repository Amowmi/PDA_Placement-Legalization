#pragma once
#include <iostream>
#include <string>
#include <vector>

using namespace std;

class Block {
   public:
    const int idx;
    int w;
    int h;
    int x;
    int y;
    

    Block() = default;
    Block(const int idx, int w, int h, int x, int y)
        : idx(idx), w(w), h(h), x(x), y(y) {}
};
