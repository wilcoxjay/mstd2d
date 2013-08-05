#include "mstd2d.h"
#include "DenseSet2d.h"
#include "SparseSet2d.h"

#include <G3D/Random.h>
#include <iostream>
#include <iomanip>

void make_set(int x, int y, SparseSet2d& s, G3D::Random& r) {
    s.clear();

    G3D::uint32 data = r.bits();
    int count = 0;
    int i, j;
    for (i = 0; i < x; ++i) {
        for (j = 0; j < y; ++j) {
            if (count % 32 == 31) {
                data = r.bits();
            }

            if ((data & 1) != 0) {
                s.add(i, j);
            }

            data >>= 1;
            count++;
        }
    }
}

#include <sys/time.h>
#include <omp.h>
#include <cstdlib>
#include <cassert>
int main(int argc, char** argv) {
    if (argc < 3) {
        std::cout << "need x and y on command line." << std::endl;
        exit(1);
    }
    int X = atoi(argv[1]);
    int Y = atoi(argv[2]);
    assert(1 <= X && X <= 1000);
    assert(1 <= Y && Y <= 1000);

    long long count = 0;
    long long trials = 0;

    #pragma omp parallel
    {
        timeval start, now;
        gettimeofday(&start, 0);

        const int tid = omp_get_thread_num();

        // intentional overflow
        G3D::uint32 seed = (G3D::uint32)(start.tv_sec + (13 * start.tv_usec) +
                                         (1299673 * 1299673 * (tid+1)));  
        #pragma omp critical 
        {
            std::cout << "thread " << std::setw(2) << std::right << tid
                      << " starting up with seed " << seed 
                      << std::endl;
        }
            
        G3D::Random r(seed, false);
    
        int report_freq = 1000000;

        long long my_count = 0;
        long long count_to_add = 0;

        int sumsize, diffsize;
        DenseSet2d sum_set(2*X + 1, 2*Y + 1);
        DenseSet2d diff_set(2*X + 1, 2*Y + 1);
        SparseSet2d my_set;

        long long my_trials = 0;
        long long trials_to_add = 0;
        while (1) {
            trials_to_add++;
            if (trials_to_add % report_freq == 0) {
                gettimeofday(&now, 0);

                #pragma omp critical 
                {
                    my_count += count_to_add;
                    count += count_to_add;
                    count_to_add = 0;

                    my_trials += trials_to_add;
                    trials += trials_to_add;
                    trials_to_add = 0;

                    std::cerr << "tid "
                              << std::setw(2) << std::right
                              << tid
                              << " itr "
                              << std::setw(15) << std::left
                              << my_trials
                              << " sec = " 
                              << std::setw(10) << std::left
                              << (now.tv_sec - start.tv_sec)
                              << " local prop = "
                              << std::setw(15) << std::left
                              << ((double)my_count)/((double)my_trials)
                              << " global prop = "
                              << ((double)count)/((double)trials)
                              << std::endl;
                }
            }

            make_set(X, Y, my_set, r);
            compute_sizes(my_set, sum_set, diff_set, sumsize, diffsize);
            if (sumsize > diffsize) {
                count_to_add++;
            }
        }
    }

    //std::cout << "proportion mstd found: " << ((double)count) / my_trials << std::endl;

    // int id;
    // unsigned long long step;
    // if (argc == 3) {
    //     id = atoi(argv[1]);
    //     int max = atoi(argv[2]);
    //     step = ((UPPER_BOUND + max - 1) / max);
    // } else {
    //     // default to searching whole space
    //     id = 0;
    //     step = UPPER_BOUND;  
    // }
    // 
    // unsigned long long i;
    // unsigned long long start = id * step;
    // unsigned long long end = (id + 1) * step;
    // std::cout << "thread id " << id << "searching from "
    //           << start << " to " << end << std::endl;
    // for (i = start; i < end ; ++i) {
    //     make_set(my_set, i);
    //     if (is_mstd(my_set)) {
    //         my_set.print();
    //         std::cout << "    (with seed " << i << ")" << std::endl;
    //     }
    // }
    return 0;
}
