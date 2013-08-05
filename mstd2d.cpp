#include "mstd2d.h"

#include "DenseSet2d.h"
#include "SparseSet2d.h"

#include <iostream>

#include <cstdlib>


void compute_sizes(const SparseSet2d& set,
                   DenseSet2d& sum_set, DenseSet2d& diff_set,
                   int& sum_size, int& diff_size) {
    sum_set.clear();
    diff_set.clear();

    const int X = diff_set.x()/2;
    const int Y = diff_set.y()/2;

    typedef SparseSet2d::Point2 Point2;

    const Point2* data = set.getPoints();
    for (int i = 0; i < set.size(); ++i) {
        const Point2& p1 = data[i];
        for (int j = i; j < set.size(); ++j) {
            const Point2& p2 = data[j];

            char sx = p1.x + p2.x;
            char d1x = p1.x - p2.x;

            char sy = p1.y + p2.y;
            char d1y = p1.y - p2.y;


            sum_set.add(sx,sy);

            diff_set.add(d1x + X, d1y + Y);
            diff_set.add(X - d1x, Y - d1y);
        }
    }

    int sums = 0;
    int diffs = 0;
    char *sum_data = sum_set.unsafe_get_data();
    char *diff_data = diff_set.unsafe_get_data();
    for (int i = 0; i < sum_set.x() * sum_set.y(); ++i) {
        sums += sum_data[i];
        diffs += diff_data[i];
    }

    sum_size = sums;
    diff_size = diffs;
    
}

bool is_mstd(const SparseSet2d& set, DenseSet2d& sum_set, DenseSet2d& diff_set) {
    int sums, diffs;
    compute_sizes(set, sum_set, diff_set, sums, diffs);
    return sums > diffs;
}
