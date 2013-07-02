/**
g++ -DNDEBUG -O3 -march=native -mtune=native -Wall -Wextra -Werror -o mstd2d mstd2d.cpp

 */

#include "Set2d.h"

#include <iostream>

#include <cstdlib>

#define XSIZE 5
#define YSIZE 5
#define DOUBLE(x) (2*x + 1)

static Set2d<DOUBLE(XSIZE),DOUBLE(YSIZE)> sum_set;
static Set2d<DOUBLE(XSIZE),DOUBLE(YSIZE)> diff_set;

template <unsigned int X, unsigned int Y>
int is_mstd(Set2d<X,Y>& set) {
    sum_set.clear();
    diff_set.clear();

    unsigned int i1, j1, i2, j2;
    for (i1 = 0; i1 < X; ++i1) {
        for (j1 = 0; j1 < Y; ++j1) {
            if (!set.contains(i1, j1)) {
                continue;
            }
            for (i2 = i1; i2 < X; ++i2) {
                int s1  = i1 + i2;
                int d11 = i1 - i2 + X;
                int d21 = i2 - i1 + X;
        
                for (j2 = 0; j2 < Y; ++j2) {
                    if (!set.contains(i2, j2)) {
                        continue;
                    }
                    int s2  = j1 + j2;
                    int d12 = j1 - j2 + Y;
                    int d22 = j2 - j1 + Y;

                    sum_set.add(s1,s2);
                    diff_set.add(d11,d12);
                    diff_set.add(d21,d22);
                }
            }
        }
    }

    int sums = 0;
    int diffs = 0;
    char *sum_data = sum_set.unsafe_get_data();
    char *diff_data = diff_set.unsafe_get_data();
    for (unsigned int i = 0; i < DOUBLE(XSIZE) * DOUBLE(YSIZE); ++i) {
        sums += sum_data[i];
        diffs += diff_data[i];
    }

    return sums > diffs;
}

template <unsigned int X, unsigned int Y>
void make_set(Set2d<X,Y>& s, unsigned long long seed) {
    s.clear();

    unsigned int i, j;
    for (i = 0; i < X; ++i) {
        for (j = 0; j < Y; ++j) {
            int k = Y*i + j;
            if ((seed >> k) & 1) {
                s.add(i, j);
            }
        }
    }
}

static Set2d<XSIZE, YSIZE> my_set;

#define UPPER_BOUND (1LL << (XSIZE*YSIZE))

int main(int argc, char **argv) {
    int id;
    unsigned long long step;
    if (argc == 3) {
        id = atoi(argv[1]);
        int max = atoi(argv[2]);
        step = ((UPPER_BOUND + max - 1) / max);
    } else {
        // default to searching whole space
        id = 0;
        step = UPPER_BOUND;  
    }

    unsigned long long i;
    unsigned long long start = id * step;
    unsigned long long end = (id + 1) * step;
    std::cout << "thread id " << id << "searching from "
              << start << " to " << end << std::endl;
    for (i = start; i < end ; ++i) {
        make_set(my_set, i);
        if (is_mstd(my_set)) {
            my_set.print();
            std::cout << "    (with seed " << i << ")" << std::endl;
        }
    }
    return 0;
}
