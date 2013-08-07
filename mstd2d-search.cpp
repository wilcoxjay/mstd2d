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
#include <climits>


int X, Y;
int num_groups = 3;
int bias_low = 0;
int bias_high = -1;
long long iters = LLONG_MAX;

void parse_args(int argc, char** argv) {
    argc--; argv++;
    if (argc < 2) {
        std::cout << "need x and y on command line." << std::endl;
        exit(1);
    }
    X = atoi(argv[0]);
    argc--; argv++;
    Y = atoi(argv[0]);
    argc--; argv++;

    while (argc > 0) {
        if (strcmp(argv[0], "--groups") == 0) {
            argc--; argv++;
            assert(argc > 0);
            num_groups = atoi(argv[0]);
            argc--; argv++;
        } else if (strcmp(argv[0], "--bias-low") == 0) {
            argc--; argv++;
            assert(argc > 0);
            bias_low = atoi(argv[0]);
            argc--; argv++;
        } else if (strcmp(argv[0], "--bias-high") == 0) {
            argc--; argv++;
            assert(argc > 0);
            bias_high = atoi(argv[0]);
            argc--; argv++;
        } else if (strcmp(argv[0], "--iters") == 0) {
            argc--; argv++;
            assert(argc > 0);
            iters = strtoll(argv[0], NULL, 10);
            argc--; argv++;
        } else {
            std::cout << "unknown option: " << argv[0] << std::endl;
            exit(1);
        }
    }

    if (bias_high == -1) {
        bias_high = X * Y;
    }

    assert(1 <= X && X <= 1000);
    assert(1 <= Y && Y <= 1000);
    assert(1 <= num_groups && num_groups <= 10);
    assert(0 <= bias_low && bias_low < bias_high && bias_high <= X*Y);

}

int main(int argc, char** argv) {
    parse_args(argc, argv);

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
        G3D::uint32 seed = (G3D::uint32)((unsigned int)start.tv_sec +
                                         (13U * (unsigned int)start.tv_usec) +
                                         (1299673U * (1299673U * (unsigned int)(tid+1))));  
        #pragma omp critical 
        {
            std::cout << "thread " << std::setw(2) << std::right << tid
                      << " starting up with seed " << seed 
                      << std::endl;
        }
            
        G3D::Random r(seed, false);
    
        long long report_freq = 1000000;

        long long my_count = 0;
        long long count_to_add = 0;

        int sumsize, diffsize;
        DenseSet2d sum_set(2*X + 1, 2*Y + 1);
        DenseSet2d diff_set(2*X + 1, 2*Y + 1);
        SparseSet2d my_set;

        int index = tid % num_groups;

        long long my_trials = 0;
        long long trials_to_add = 0;
        while (my_trials < iters) {
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
            if (bias_low <= my_set.size() && my_set.size() <= bias_high) {
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
        #pragma omp critical
        {
            std::cout << "tid " << tid << " exiting." << std::endl;
        }
    }
    return 0;
}
