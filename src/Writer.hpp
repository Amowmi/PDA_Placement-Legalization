#pragma once
#include <algorithm>
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <iomanip>

#include "DataMgr.hpp"
using namespace std;

class Writer {
    string output_path;
    ifstream output_file;
    DataMgr::ptr dataMgr;
    double total_DP;
    double MDP;

   public:
    Writer(const string& output_path, DataMgr::ptr dataMgr)
        : output_path(output_path), dataMgr(move(dataMgr)) {}

    unique_ptr<DataMgr> write();
    void writeOutput();
    void computeDP();
};