#include <vector>
#include <iostream>
#include <cassert>
#include <cstdlib>

#include "rat.h"
#include "vec.h"
#include "set.h"
#include "setset.h"

bool contains(const std::vector< vec<DIM> >& set, const vec<DIM>& v) {
    for (auto i : set) {
        if (i == v) {
            return true;
        }
    }
    return false;
}

void add(std::vector< vec<DIM> >& set, const vec<DIM>& v) {
    if (!contains(set, v)) {
        set.push_back(v);
    }
}

bool is_mstd(const set& s) {
    static std::vector< vec<DIM> > sumset;
    static std::vector< vec<DIM> > diffset;
    sumset.clear();
    diffset.clear();
    
    for (int i = 0; i < DIM; ++i) {
        for (int j = i; j < DIM; ++j) {
            add(sumset, s[i] + s[j]);
            add(diffset, s[i] - s[j]);
            add(diffset, s[j] - s[i]);
        }
    }

    // std::cout << "sumset = " << std::endl;
    // for (auto v : sumset) {
    //     std::cout << "  " << v << std::endl;
    // }
    // std::cout << "diffset = " << std::endl;
    // for (auto v : diffset) {
    //     std::cout << "  " << v << std::endl;
    // }

    // std::cout << "sumset.size = "  << sumset.size()  << std::endl;
    // std::cout << "diffset.size = " << diffset.size() << std::endl;

    return sumset.size() > diffset.size();
}



void search(const set& start) {
    setset a, b;
    setset* current = &a;
    setset* next    = &b;

    current->add(start);

    for (int depth = 0; depth < DIM; ++depth) {
        std::cerr << "starting iteration." << std::endl
                  << "    depth        = " << depth           << std::endl
                  << "    current.size = " << current->size() << std::endl
                  << "    next.size    = " << next->size()
                  << std::endl;
        for (auto s : *current) {
            for (int i = 0;   i < DIM; ++i) {
            for (int j = i+1; j < DIM; ++j) {
            for (int k = j+1; k < DIM; ++k) {
            for (int l = k+1; l < DIM; ++l) {

                vec<DIM> v = (s[i] - s[j]) - (s[k] - s[l]);

                if (v.is_zero()) continue;

                assert(!(v.has_NaN() || v.has_infty()));

                set t = s;
                for (int x = 0; x < DIM; ++x) {
                    t[x] -= t[x].proj_onto(v);
                }
                t.projections++;

                next->add(t);
            }
            }
            }
            }
        }
        current->clear();
        std::swap(current, next);

        std::cout << current->size() << " sets of depth " << depth << std::endl
                  << *current << std::endl;

        for (auto i : *current) {
            if (is_mstd(i)) {
                std::cout << "MSTD set!" << std::endl;
                std::cout << i << std::endl;
            }
        }
    }
    
}

int main() {
    set my_set;

    for (int i = 0; i < DIM; ++i) {
        my_set[i][i] = rat(1);
    }

    // std::cout << my_set << std::endl;
    //
    // vec<DIM> v = my_set[0] - my_set[1] - my_set[2] + my_set[3];
    // for (int i = 0; i < DIM; ++i) {
    //     my_set[i] -= my_set[i].proj_onto(v);
    // }
    //
    // std::cout << my_set << std::endl;
    //
    // v = my_set[0] - my_set[1] - my_set[2] + my_set[4];
    // for (int i = 0; i < DIM; ++i) {
    //     const vec<DIM> w = my_set[i].proj_onto(v);
    //     std::cout << "    " << my_set[i] << " project-> " << v << " = " << w << std::endl;
    //     my_set[i] -= w;
    // }
    // std::cout << my_set << std::endl;

    search(my_set);

    return 0;
}
