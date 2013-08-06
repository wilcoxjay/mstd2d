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

    int num_groups = 3;

    long long count[num_groups];
    long long trials[num_groups];
    int i;
    for (i = 0; i < num_groups; i++) {
        count[i] = 0;
        trials[i] = 0;
    }

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

        int index = tid % num_groups;

        long long my_trials = 0;
        long long trials_to_add = 0;
        while (1) {
            trials_to_add++;
            if (trials_to_add % report_freq == 0) {
                gettimeofday(&now, 0);

                #pragma omp critical 
                {
                    my_count += count_to_add;
                    count[index] += count_to_add;
                    count_to_add = 0;

                    my_trials += trials_to_add;
                    trials[index] += trials_to_add;
                    trials_to_add = 0;

                    long long glob_count  = 0;
                    long long glob_trials = 0;
                    int j;
                    for (j = 0; j < num_groups; j++) {
                        glob_count += count[j];
                        glob_trials += trials[j];
                    }

                    std::cout << "tid "
                              << std::setw(2) << std::right
                              << tid
                              << " group "
                              << index
                              << " itr "
                              << std::setw(10) << std::left
                              << my_trials
                              << " sec = " 
                              << std::setw(6) << std::left
                              << (now.tv_sec - start.tv_sec)
                              << " local = "
                              << std::setw(12) << std::left
                              << ((double)my_count)/((double)my_trials)
                              << " group = "
                              << std::setw(12) << std::left
                              << ((double)count[index])/((double)trials[index])
                              << " global = "
                              << ((double)glob_count)/((double)glob_trials)
                              << std::endl;
                }
            }

            make_set(X, Y, my_set, r);
            compute_sizes(my_set, sum_set, diff_set, sumsize, diffsize);
            if (sumsize > diffsize) {
                count_to_add++;
                #pragma omp critical
                {
                    std::cout << "(" << my_set.size() << "," << sumsize << "," << diffsize << ") "
                              << my_set << std::endl;
                }
            }
        }
    }

    return 0;
}
