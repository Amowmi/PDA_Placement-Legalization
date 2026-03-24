#pragma once
#include <memory>
#include <vector>
#include <unordered_map>

#include "Cell.hpp"
#include "Row.hpp"
#include "Block.hpp"
using namespace std;

class DataMgr {
   public:
    using ptr = std::unique_ptr<DataMgr>;

    int MDC; 
    size_t cheat;
    vector<Cell*> AllCells;
    vector<Row*> AllRows;
    vector<Block*> AllBlocks;
    DataMgr() = default;
};