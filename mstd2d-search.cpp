#include "mstd2d.h"
#include "SparseSet2d.h"

static SparseSet2d my_set;

#include <G3D/Random.h>
#include <iostream>

void make_set(SparseSet2d& s, G3D::Random& r) {
    s.clear();

    G3D::uint32 data = r.bits();
    int count = 0;
    unsigned int i, j;
    for (i = 0; i < XSIZE; ++i) {
        for (j = 0; j < YSIZE; ++j) {
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

#define UPPER_BOUND (1LL << (XSIZE*YSIZE))
#include <sys/time.h>
int main() {
    timeval t;
    gettimeofday(&t, 0);

    G3D::Random r((G3D::uint32)t.tv_sec + t.tv_usec);
    
    //int num_trials = 1000000;
    int report_freq = 1000000;
    int count = 0;

    int sumsize, diffsize;

    long long i;
    for (i = 0; /*i < 2*/; ++i) {
        if (i % report_freq == 0) {
            std::cerr << "beginning iteration " << i
                      << std::endl;
        }

        make_set(my_set, r);
        compute_sizes(my_set, sumsize, diffsize);
        if (sumsize > diffsize) {
            count++;
            std::cout << "set found! new proportion = " << ((double)count)/(i+1) << std::endl;
            std::cout << my_set << std::endl;
            std::cout << "  of size " << my_set.size()
                      << " sumset size = " << sumsize
                      << " diffset size = " << diffsize << std::endl;
                
            std::cout.flush();
        }

    }

    //std::cout << "proportion mstd found: " << ((double)count) / i << std::endl;

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
