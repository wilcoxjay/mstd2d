/**
g++ -DNDEBUG -O3 -march=native -mtune=native -Wall -Wextra -Werror -Wconversion -o mstd2d mstd2d.cpp

 */

#include "mstd2d.h"

#include "DenseSet2d.h"
#include "SparseSet2d.h"

#include <iostream>

#include <cstdlib>

static DenseSet2d<DOUBLE(XSIZE),DOUBLE(YSIZE)> sum_set;
static DenseSet2d<DOUBLE(XSIZE),DOUBLE(YSIZE)> diff_set;

#include <sys/time.h>
timeval start, end;
long long my_time = 0;

inline void compute_sizes(const SparseSet2d& set, int& sum_size, int& diff_size) {
    sum_set.clear();
    diff_set.clear();

    typedef SparseSet2d::Point2 Point2;

    gettimeofday(&start, 0);
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
            diff_set.add(d1x+XSIZE,d1y+YSIZE);
            diff_set.add(XSIZE-d1x,YSIZE-d1y);
        }
    }
    gettimeofday(&end, 0);

    my_time += (end.tv_usec - start.tv_usec);
    my_time >>= 1;

    int sums = 0;
    int diffs = 0;
    char *sum_data = sum_set.unsafe_get_data();
    char *diff_data = diff_set.unsafe_get_data();
    for (unsigned int i = 0; i < DOUBLE(XSIZE) * DOUBLE(YSIZE); ++i) {
        sums += sum_data[i];
        diffs += diff_data[i];
    }

    sum_size = sums;
    diff_size = diffs;
    
}

bool is_mstd(const SparseSet2d& set) {
    int sums, diffs;
    compute_sizes(set, sums, diffs);
    return sums > diffs;
}
