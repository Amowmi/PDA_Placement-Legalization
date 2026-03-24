#pragma once
#include <iostream>
#include <string>
#include <vector>

#include "Subrow.hpp"

using namespace std;

class Row {
   public:
    const int idx;
    int site_width;
    int height;
    int x;
    int y;
    int site_number;
    vector<Subrow*> subrows; 

    Row() = default;
    Row(const int idx, int site_width, int height, int x, int y,
        int site_number)
        : idx(idx),
          site_width(site_width),
          height(height),
          x(x),
          y(y),
          site_number(site_number) {}
          
    int letAlign(int x, bool left){
      if(x % site_width == 0) return x;
      if(left){
        return x - (x % site_width);
      }
      else{
        return x - (x % site_width) + site_width;
      }
    }
};
