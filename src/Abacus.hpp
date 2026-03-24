#pragma once
#include <omp.h>

#include <algorithm>
#include <cassert>
#include <chrono>
#include <climits>
#include <cmath>
#include <memory>
#include <random>
#include <stack>
#include <array>

#include "Block.hpp"
#include "Cell.hpp"
#include "Cluster.hpp"
#include "DataMgr.hpp"
#include "Row.hpp"
#include "Subrow.hpp"

using namespace std;

class Abacus {
   public:
    int tmp = 0;

    clock_t start_time;
    double duration;
    mt19937 rng;
    DataMgr::ptr dataMgr;

    Abacus(DataMgr::ptr dataMgr) : dataMgr(move(dataMgr)) {}

    unique_ptr<DataMgr> solve();

    void createSubrows();
    int findNearRow(Cell*);
    void abacus();
    pair<int, double> tryPlaceRow(Cell*, int);
    void placeRow(Cell*, int, int);
    array<int, 3> findBestSub(Cell*, Row*);
    void alignCell();
    void microSwap();

    int rowNeddFix(Row*);
};