#include "Writer.hpp"
using namespace std;

unique_ptr<DataMgr> Writer::Writer::write() {
    ofstream output_file(output_path, ios::out);

    if (output_file.fail()) {
        cout << "output file opening failed" << endl;
        exit(1);
    }
    writeOutput();
    return move(dataMgr);
}

inline void Writer::writeOutput() {
    ofstream output_file;
    output_file.open(output_path);
    computeDP();


    output_file << std::fixed << std::setprecision(0);
    output_file << "TotalDisplacement " << ceil(total_DP) << '\n';
    output_file << "MaxDisplacement " << ceil(MDP) << '\n';
    output_file << "NumCells " << dataMgr->AllCells.size() << '\n';
    for (auto& cell : dataMgr->AllCells) {
        output_file << "c" << cell->idx << " " << cell->legal_x << " " << cell->legal_y
                    << '\n';
    }

    output_file.close();
}

void Writer::computeDP() {
    MDP = 0.0;
    total_DP = 0.0;
    for (auto& cell : dataMgr->AllCells) {
        double dp = cell->getDistance();
        total_DP += dp;
        if (dp > MDP) {
            MDP = dp;
        }
    }
    cout << std::fixed << std::setprecision(0);
    // cout << "total DP : " << total_DP << endl;
}