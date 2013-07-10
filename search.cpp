#include <iostream>
#include <cassert>
#include <cstdlib>

#include "rat.h"
#include "vec.h"


#define DIM 7

class set {
private:
    vec<DIM> data[DIM];
public:
    int projections;

    set() : projections(0) { }

    inline const vec<DIM>& operator[](int i) const { return data[i]; }
    inline       vec<DIM>& operator[](int i)       { return data[i]; }
};

std::ostream& operator<<(std::ostream& os, const set& s) {
    bool started = false;
    os << "{" << std::endl;;
    for (int i = 0; i < DIM; ++i) {
        if (started) {
            os << "," << std::endl;
        }
        started = true;
        os << "    " << s[i];
    }

    os << "  | " << s.projections << std::endl << "}";

    return os;
}




void search(const set& s) {
    static long long calls = 0;
    calls++;

    for (int i = 0;   i < DIM; ++i) {
    for (int j = i+1; j < DIM; ++j) {
    for (int k = j+1; k < DIM; ++k) {
    for (int l = k+1; l < DIM; ++l) {
        if (s.projections < 2) {
            std::cerr << "depth = " << s.projections << " calls = " << calls  << " i = " << i << " j = " << j << " k = " << k << " l = " << l << std::endl;
        }

        vec<DIM> v = (s[i] - s[j]) - (s[k] - s[l]);
        
        if (v.is_zero()) continue;

        if (v.has_NaN() || v.has_infty()) {
            std::cerr << "ran into badness while trying to process " << s << std::endl
                      << "while computing with i,j,k,l = "
                      << i << "," << j << "," << k << "," << l << std::endl;
            continue;
        }

        set t = s;
        for (int x = 0; x < DIM; ++x) {
            t[x] -= t[x].proj_onto(v);
        }
        t.projections++;

        if (t.projections == 2) {
            std::cout << "finished searching " << t << std::endl;
        } else {
            search(t);
        }
    }
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
