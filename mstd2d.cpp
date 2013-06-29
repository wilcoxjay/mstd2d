/**
g++ -DNDEBUG -O3 -march=native -mtune=native -Wall -Wextra -Werror -o mstd2d mstd2d.cpp

 */

#include "Set2d.h"

#include <iostream>

#include <cstdlib>

#define XSIZE 5
#define YSIZE 5
#define DOUBLE(x) (2*x + 1)

// 4x6, 3x8, 2x12

static Set2d<DOUBLE(XSIZE),DOUBLE(YSIZE)> sum_set;
static Set2d<DOUBLE(XSIZE),DOUBLE(YSIZE)> diff_set;

template <unsigned int X, unsigned int Y>
int is_mstd(Set2d<X,Y>& set) {
    sum_set.clear();
    diff_set.clear();
    //set.init_sparse();

    // for (int k = 0; k < set.m_sparse_size; ++k) {
    //     int i1 = set.m_sparse_x[k];
    //     int j1 = set.m_sparse_y[k];
    //     for (int l = k; l < set.m_sparse_size; ++l) {
    //         int i2 = set.m_sparse_x[l];
    //         int j2 = set.m_sparse_y[l];
    // 
    //         int s1  = i1 + i2;
    //         int d11 = i1 - i2 + X;
    //         int d21 = i2 - i1 + X;
    //         
    //         int s2  = j1 + j2;
    //         int d12 = j1 - j2 + Y;
    //         int d22 = j2 - j1 + Y;
    // 
    //         sum_set.add(s1,s2);
    //         diff_set.add(d11,d12);
    //         diff_set.add(d21,d22);
    //     }
    // }

    unsigned int i1, j1, i2, j2;
    for (i1 = 0; i1 < X; ++i1) {
        for (j1 = 0; j1 < Y; ++j1) {
            if (!set.contains(i1, j1)) {
                continue;
            }
            //printf ("(%d,%d) is in the set.\n", i1, j1);
            for (i2 = i1; i2 < X; ++i2) {
                int s1  = i1 + i2;
                int d11 = i1 - i2 + X;
                int d21 = i2 - i1 + X;
        
                for (j2 = 0; j2 < Y; ++j2) {
                    if (!set.contains(i2, j2)) {
                        continue;
                    }
                    //printf ("(%d,%d) is in the set.\n", i2, j2);
                    int s2  = j1 + j2;
                    int d12 = j1 - j2 + Y;
                    int d22 = j2 - j1 + Y;

                    // printf("(%d,%d) + (%d,%d) = (%d,%d)\n", i1, j1, i2, j2, s1, s2);
                    // printf("(%d,%d) - (%d,%d) = (%d,%d)\n", i1, j1, i2, j2, d11-X, d12-Y);
                    // printf("(%d,%d) - (%d,%d) = (%d,%d)\n", i2, j2, i1, j1, d21-X, d22-Y);

                    // printf("s(%d,%d)\n", s1, s2);
                    // printf("d(%d,%d)\n", d11-N, d12-N);
                    // printf("d(%d,%d)\n", d21-N, d22-N);


                    // if (!sum_set.contains(s1,s2) ||
                    //     !diff_set.contains(d11,d12) ||
                    //     !diff_set.contains(d21,d22)) {
                    //     std::cout << "error on ("
                    //               << i1 << ","
                    //               << j1 << ") ("
                    //               << i2 << ","
                    //               << j1 << ")"
                    //               << std::endl;
                    // }

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

    //int sparse_ind = 0;

    unsigned int i, j;
    for (i = 0; i < X; ++i) {
        for (j = 0; j < Y; ++j) {
            int k = Y*i + j;
            if ((seed >> k) & 1) {
                // s.m_sparse_x[sparse_ind] = i;
                // s.m_sparse_y[sparse_ind] = j;
                // sparse_ind++;
                s.add(i, j);
            }
        }
    }

    //s.m_sparse_size = sparse_ind;
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
        //if (i % 100000 == 0) {
        //    std::cout << "iteration " << i << std::endl;
        //}
        make_set(my_set, i);
        if (is_mstd(my_set)) {
            my_set.print();
            std::cout << "    (with seed " << i << ")" << std::endl;
        }
    }
    return 0;
}
