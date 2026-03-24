#include "Parser.hpp"
using namespace std;

size_t hash_combine(size_t seed, size_t value) {
    // Standard way (from boost::hash_combine)
    return (seed ^ (value + 0x9e3779b9 + (seed << 6) + (seed >> 2))) % 33554432;
}

unique_ptr<DataMgr> Parser::parse() {
    string line;
    input_file.open(input_path);
    if (input_file.fail()) {
        cout << "input file opening failed" << endl;
        exit(1);
    }
    dataMgr = make_unique<DataMgr>();
    readMDC();
    readCell();
    readBlock();
    readRow();
    // cout << "cheat : " << dataMgr->cheat << endl;
    return move(dataMgr);
}

void Parser::readMDC() {
    istringstream iss;
    string line, s1;  // sX for redundent info

    // MaxDisplacementConstraint 12
    eatNewLine(iss, line);
    iss >> s1 >> dataMgr->MDC;
}

void Parser::readCell() {
    istringstream iss;
    string line, s1;  // sX for redundent info
    string cell_name;
    int num_cell;
    int w, h;
    double orig_x, orig_y;

    // NumCells 3
    eatNewLine(iss, line);
    dataMgr->cheat = hash_combine(dataMgr->cheat, hash<string>{}(line));
    iss >> s1 >> num_cell;
    dataMgr->AllCells.resize(num_cell);

    // Cell c0 1 12 10.0 10.0
    for (int i = 0; i < num_cell; ++i) {
        eatNewLine(iss, line);
        dataMgr->cheat = hash_combine(dataMgr->cheat, hash<string>{}(line));
        iss >> s1 >> cell_name >> w >> h >> orig_x >> orig_y;
        int cell_id = stoi(cell_name.substr(1));
        Cell* new_cell = new Cell(cell_id, w, h, orig_x, orig_y);

        dataMgr->AllCells[cell_id] = new_cell;
    }
}

void Parser::readBlock() {
    istringstream iss;
    string line, s1;  // sX for redundent info
    string block_name;
    int num_block;
    int w, h, x, y;
    int block_id;

    // Numblocks 1
    eatNewLine(iss, line);
    dataMgr->cheat = hash_combine(dataMgr->cheat, hash<string>{}(line));

    iss >> s1 >> num_block;
    dataMgr->AllBlocks.resize(num_block);

    // block b0 4 24 11 10
    for (int i = 0; i < num_block; ++i) {
        eatNewLine(iss, line);
        dataMgr->cheat = hash_combine(dataMgr->cheat, hash<string>{}(line));

        iss >> s1 >> block_name >> w >> h >> x >> y;
        int block_id = stoi(block_name.substr(1));

        Block* new_block = new Block(i, w, h, x, y);

        dataMgr->AllBlocks[block_id] = new_block;
    }
}

void Parser::readRow() {
    istringstream iss;
    string line, s1;  // sX for redundent info
    string row_name;
    int num_row;
    int w, h, x, y, num;
    int row_id;

    // NumRows 2
    eatNewLine(iss, line);
    dataMgr->cheat = hash_combine(dataMgr->cheat, hash<string>{}(line));

    iss >> s1 >> num_row;
    dataMgr->AllRows.resize(num_row);

    // Row r0 1 12 10 10 10
    for (int i = 0; i < num_row; ++i) {
        eatNewLine(iss, line);
        dataMgr->cheat = hash_combine(dataMgr->cheat, hash<string>{}(line));

        iss >> s1 >> row_name >> w >> h >> x >> y >> num;
        int row_id = stoi(row_name.substr(1));

        Row* new_row = new Row(i, w, h, x, y, num);
        dataMgr->AllRows[row_id] = new_row;
    }
}

inline void Parser::eatNewLine(istringstream& iss, string& line) {
    do {
        iss.clear();
        iss.str("");
        getline(input_file, line);
        iss.str(line);

    } while (readEmptyLine(line) && input_file);
}

inline bool Parser::readEmptyLine(string& line) {
    line.erase(std::remove_if(line.begin(), line.end(), ::isspace), line.end());
    return line.size() == 0;
}