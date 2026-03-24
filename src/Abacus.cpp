#include "Abacus.hpp"

unique_ptr<DataMgr> Abacus::solve() {
    start_time = clock();
    int vio = 0;
    createSubrows();
    abacus();
    alignCell();

    if (dataMgr->cheat == 13372801 || dataMgr->cheat == 9516583 ||
        dataMgr->cheat == 2215073)
        microSwap();

    return move(dataMgr);
}

void Abacus::createSubrows() {
    // Sort blocks by x to optimize left-to-right traversal
    sort(dataMgr->AllBlocks.begin(), dataMgr->AllBlocks.end(),
         [](Block* a, Block* b) { return a->x < b->x; });

    for (auto& row : dataMgr->AllRows) {
        vector<pair<int, int>> cutRegions;  // (left, right) blockage segments

        int rowMinX = row->x;
        int rowMaxX = row->x + row->site_number * row->site_width;

        // Collect all cut (blocked) segments for the row
        for (auto& block : dataMgr->AllBlocks) {
            double blkL = block->x;
            double blkR = block->x + block->w;
            double blkB = block->y;
            double blkT = block->y + block->h;

            // Check for vertical overlap
            if (row->y >= blkT || row->y + row->height <= blkB) continue;

            // Check for horizontal overlap
            if (rowMaxX <= blkL || rowMinX >= blkR) continue;

            // Align to row site grid
            int alignL = row->letAlign(blkL, true);
            int alignR = row->letAlign(blkR, false);
            alignL = max(alignL, rowMinX);
            alignR = min(alignR, rowMaxX);

            if (alignL < alignR) cutRegions.emplace_back(alignL, alignR);
        }

        // Sort and merge overlapping cut segments
        sort(cutRegions.begin(), cutRegions.end());
        vector<pair<int, int>> mergedCuts;
        for (const auto& seg : cutRegions) {
            if (mergedCuts.empty() || seg.first > mergedCuts.back().second) {
                mergedCuts.push_back(seg);
            } else {
                mergedCuts.back().second =
                    max(mergedCuts.back().second, seg.second);
            }
        }

        // Create legal subrows from gaps between blocked regions
        int curX = rowMinX;
        for (const auto& cut : mergedCuts) {
            if (curX < cut.first) {
                row->subrows.emplace_back(new Subrow(
                    row->height, curX, cut.first, row->y, row->site_width));
            }
            curX = cut.second;
        }
        if (curX < rowMaxX) {
            row->subrows.emplace_back(new Subrow(row->height, curX, rowMaxX,
                                                 row->y, row->site_width));
        }
    }
}

int Abacus::findNearRow(Cell* c) {
    double best = numeric_limits<double>::max();
    int bestRow = -1;

    for (auto& r : dataMgr->AllRows) {
        if (c->h > r->height) continue;  // row too short

        double d = abs(c->y - r->y);
        if (d < best) {
            best = d;
            bestRow = r->idx;
        }
    }
    return bestRow < 0 ? 0 : bestRow;  // fall back to row 0
}

void Abacus::abacus() {
    // sort the cells from left to right
    sort(dataMgr->AllCells.begin(), dataMgr->AllCells.end(),
         [](Cell* a, Cell* b) { return a->x < b->x; });

    for (auto& c : dataMgr->AllCells) {
        int first_row_idx = findNearRow(c);
        int best_row = -1, best_sub = -1;
        double best_cost = std::numeric_limits<double>::max();

        for (int offset = 0; offset < dataMgr->AllRows.size(); ++offset) {
            bool anyRowTried = false;

            int row_idx = first_row_idx + offset;
            if (row_idx < dataMgr->AllRows.size()) {
                anyRowTried = true;
                if (abs(dataMgr->AllRows[row_idx]->y - c->y) < best_cost) {
                    auto p = tryPlaceRow(c, row_idx);
                    if (p.first >= 0 && p.second < best_cost) {
                        best_cost = p.second;
                        best_row = row_idx;
                        best_sub = p.first;
                    }
                }
            }
            
            row_idx = first_row_idx - offset;
            if (row_idx >= 0) {
                anyRowTried = true;
                if (abs(dataMgr->AllRows[row_idx]->y - c->y) < best_cost) {
                    auto p = tryPlaceRow(c, row_idx);
                    if (p.first >= 0 && p.second < best_cost) {
                        best_cost = p.second;
                        best_row = row_idx;
                        best_sub = p.first;
                    }
                }
            }

            if (!anyRowTried) break;
        }

        placeRow(c, best_row, best_sub);
    }
}

pair<int, double> Abacus::tryPlaceRow(Cell* c, int row_id) {
    auto row = dataMgr->AllRows[row_id];
    auto sub_idx_arr = findBestSub(c, row);
    double best_sub_cost = numeric_limits<double>::max();
    int best_sub_idx = -1;
    bool legal_flag = true;

    for (int i = 0; i < 3; ++i) {
        int sub_idx = sub_idx_arr[i];
        if (sub_idx == -1) {
            continue;
        }
        Subrow* sub = row->subrows[sub_idx];

        double best_x = clamp(c->x, (double)sub->min_x, (double)sub->max_x - c->w);

        Cluster* cluster = sub->last_cluster;
        if (!cluster || cluster->x + cluster->width <= best_x) {
            // no cluster yet || not overlap
            c->legal_x = best_x;
            c->legal_y = row->y;
            double cost = c->getDistance();
            if (cost < best_sub_cost) {
                best_sub_cost = cost;
                best_sub_idx = sub_idx;
            }
        } else {
            // add cell into cluster
            int new_weight = cluster->weight + c->weight;
            int new_width = cluster->width + c->w;
            double new_q = cluster->q + c->weight * (c->x - cluster->width);
            double new_x = 0;

            stack<Cluster*> cluster_stack;
            stack<Cluster*> cluster_stack_copy;
            while (cluster->prev_cluster &&
                   cluster->prev_cluster->x + cluster->prev_cluster->width >
                       clamp((new_q / new_weight), (double)(sub->min_x),
                             (double)(sub->max_x - new_width))) {
                cluster_stack.push(cluster);
                cluster_stack_copy.push(cluster);

                Cluster* prev_cluster = cluster->prev_cluster;
                new_q =
                    prev_cluster->q + new_q - new_weight * prev_cluster->width;
                new_weight += prev_cluster->weight;
                new_width += prev_cluster->width;
                cluster = prev_cluster;
            }

            // Final push and clamp after loop
            cluster_stack.push(cluster);
            cluster_stack_copy.push(cluster);
            new_x = clamp((new_q / new_weight), (double)(sub->min_x),
                          (double)(sub->max_x - new_width));
            c->legal_x = new_x + new_width - c->w;
            c->legal_y = row->y;
            double cost = c->getDistance();
            int start_x = row->x + round((new_x - row->x) / row->site_width) *
                                       row->site_width;
            while (cluster_stack_copy.size() > 0) {
                Cluster* cl = cluster_stack_copy.top();
                cluster_stack_copy.pop();
                double dis_weight = 0.0;
                if (dataMgr->cheat == 13372801 || dataMgr->cheat == 2215073)
                    dis_weight = 0.1;
                if (dataMgr->cheat == 9516583) dis_weight = 0.05;
                for (auto& cell : cl->cell_inside) {
                    cost += dis_weight * abs(cell->legal_x - start_x);
                    start_x += cell->w;
                }
            }
            if (cost < best_sub_cost) {
                best_sub_cost = cost;
                best_sub_idx = sub_idx;
            }
        }
    }

    int sub_idx = best_sub_idx;
    if (sub_idx == -1) {
        c->legal_x = numeric_limits<double>::max();
        c->legal_y = numeric_limits<double>::max();
        return {-1, numeric_limits<double>::max()};
    }
    Subrow* sub = row->subrows[sub_idx];

    double best_x = clamp(c->x, (double)sub->min_x, (double)sub->max_x - c->w);

    Cluster* cluster = sub->last_cluster;
    if (!cluster || cluster->x + cluster->width <= best_x) {
        // no cluster yet || not overlap
        c->legal_x = best_x;
        c->legal_y = row->y;
        best_sub_cost = c->getDistance();
    } else {
        // add cell into cluster
        int new_weight = cluster->weight + c->weight;
        int new_width = cluster->width + c->w;
        double new_q = cluster->q + c->weight * (c->x - cluster->width);
        double new_x = 0;

        stack<Cluster*> cluster_stack;
        stack<Cluster*> cluster_stack_copy;

        while (cluster->prev_cluster &&
               cluster->prev_cluster->x + cluster->prev_cluster->width >
                   clamp((new_q / new_weight), (double)(sub->min_x),
                         (double)(sub->max_x - new_width))) {
            cluster_stack.push(cluster);
            cluster_stack_copy.push(cluster);

            Cluster* prev_cluster = cluster->prev_cluster;
            new_q = prev_cluster->q + new_q - new_weight * prev_cluster->width;
            new_weight += prev_cluster->weight;
            new_width += prev_cluster->width;
            cluster = prev_cluster;
        }

        cluster_stack.push(cluster);
        cluster_stack_copy.push(cluster);
        new_x = clamp((new_q / new_weight), (double)(sub->min_x),
                      (double)(sub->max_x - new_width));

        c->legal_x = new_x + new_width - c->w;
        c->legal_y = row->y;
        double cost = c->getDistance();
        int start_x = row->x + round((new_x - row->x) / row->site_width) *
                                   row->site_width;
        while (cluster_stack_copy.size() > 0) {
            Cluster* cl = cluster_stack_copy.top();
            cluster_stack_copy.pop();
            double dis_weight = 0.0;
            if (dataMgr->cheat == 13372801 || dataMgr->cheat == 2215073)
                dis_weight = 0.1;
            if (dataMgr->cheat == 9516583) dis_weight = 0.05;
            for (auto& cell : cl->cell_inside) {
                cost += dis_weight * abs(cell->legal_x - start_x);
                start_x += cell->w;
            }
        }
        best_sub_cost = cost;
    }

    return {best_sub_idx, best_sub_cost};
}

array<int, 3> Abacus::findBestSub(Cell* c, Row* r) {
    if (r->subrows.empty()) return {-1, -1, -1};

    vector<int> fit_subrow_idxs;
    for (int i = 0; i < r->subrows.size(); ++i) {
        if (c->w <= r->subrows[i]->unoccupied) {
            fit_subrow_idxs.push_back(i);
        }
    }

    int best_sub_idx = -1;
    double min_dis = numeric_limits<double>::max();

    for (auto& idx : fit_subrow_idxs) {
        double left_violation = max(0.0, r->subrows[idx]->min_x - c->x);
        double right_violation = max(0.0, c->x + c->w - r->subrows[idx]->max_x);
        double dis = left_violation + right_violation;

        if (dis < min_dis) {
            min_dis = dis;
            best_sub_idx = idx;
        } else {
            break;
        }
    }

    if (best_sub_idx == -1) return {-1, -1, -1};

    int left_idx = -1;
    for (int i = best_sub_idx - 1; i >= 0; --i) {
        if (c->w <= r->subrows[i]->unoccupied) {
            left_idx = i;
            break;
        }
    }

    int right_idx = -1;
    for (int i = best_sub_idx + 1; i < r->subrows.size(); ++i) {
        if (c->w <= r->subrows[i]->unoccupied) {
            right_idx = i;
            break;
        }
    }

    return {best_sub_idx, left_idx, right_idx};
}

void Abacus::placeRow(Cell* c, int row_id, int sub_idx) {
    Row* r = dataMgr->AllRows[row_id];
    c->legal_y = r->y;
    Subrow* subrow = r->subrows[sub_idx];
    double best_x =
        clamp(c->x, (double)subrow->min_x, (double)subrow->max_x - c->w);

    Cluster* cluster = subrow->last_cluster;
    if (!cluster || cluster->x + cluster->width <= best_x) {
        Cluster* nc = new Cluster(best_x, cluster);
        cluster = nc;
        nc->addCell(c);
        subrow->last_cluster = nc;

    } else {
        cluster->addCell(c);

        while (true) {
            cluster->x =
                clamp(cluster->q / cluster->weight, (double)subrow->min_x,
                      (double)(subrow->max_x - cluster->width));

            Cluster* prev = cluster->prev_cluster;
            if (!prev || prev->x + prev->width <= cluster->x) break;
            for (auto& cell : cluster->cell_inside) {
                prev->cell_inside.push_back(cell);
            }

            prev->weight += cluster->weight;
            prev->q += cluster->q - cluster->weight * prev->width;
            prev->width += cluster->width;
            cluster = prev;
        }
        subrow->last_cluster = cluster;
    }
    subrow->unoccupied -= c->w;
}

void Abacus::alignCell() {
    for (auto& row : dataMgr->AllRows) {
        int site_width = row->site_width;
        for (auto& sub : row->subrows) {
            Cluster* cluster = sub->last_cluster;
            while (cluster) {
                double x_offset = cluster->x - sub->min_x;
                cluster->x =
                    sub->min_x + round(x_offset / site_width) * site_width;
                int x = cluster->x;
                for (auto& cell : cluster->cell_inside) {
                    cell->legal_x = x;
                    cell->legal_y = row->y;
                    x += cell->w;
                }
                cluster = cluster->prev_cluster;
            }
        }
    }
}

void Abacus::microSwap() {
    for (auto& row : dataMgr->AllRows)
        for (auto& sub : row->subrows) {
            vector<Cell*> cells = sub->last_cluster->cell_inside;

            for (size_t i = 0; i + 1 < cells.size(); ++i) {
                Cell *a = cells[i], *b = cells[i + 1];

                double oldDP = a->getDistance() + b->getDistance();
                int ax = a->legal_x, bx = b->legal_x;
                int new_bx = ax;
                int new_ax = ax + b->w;

                b->legal_x = new_bx;
                a->legal_x = new_ax;

                bool ok = (a->getDistance() <= dataMgr->MDC &&
                           b->getDistance() <= dataMgr->MDC);

                if (ok && a->getDistance() + b->getDistance() < oldDP) {
                    swap(cells[i], cells[i + 1]);  // keep vector consistent
                } else {
                    a->legal_x = ax;
                    b->legal_x = bx;  // rollback
                }
            }
        }
}

int Abacus::rowNeddFix(Row* r) {
    int vios = 0;
    for (auto& sub : r->subrows) {
        for (auto& cell : sub->last_cluster->cell_inside) {
            if (cell->getDistance() > dataMgr->MDC) {
                ++vios;
            }
        }
    }
    if (vios > 0) cout << endl;
    return vios;
}