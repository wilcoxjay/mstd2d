#include <vector>
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

/**
   Note that this does not take into account the number of
   projections.
 */
inline bool operator==(const set& lhs, const set& rhs) {
    for (int i = 0; i < DIM; ++i) {
        if (lhs[i] != rhs[i]) {
            return false;
        }
    }
    return true;
}

inline bool operator!=(const set& lhs, const set& rhs) {
    return !operator==(lhs,rhs);
}

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


/**
   a poorly named wrapper to treat std::vector as a set. that is, it
   contains no duplicates and does not expose its order.
*/
class setset {
private:
    std::vector<set> sets;
public:
    void add(const set& s);
    bool contains(const set& s) const;
    std::vector<set>::iterator begin() {
        return sets.begin();
    }
    std::vector<set>::iterator end() {
        return sets.end();
    }
    void clear() {
        sets.clear();
    }
    int size() const {
        return sets.size();
    }

    friend std::ostream& operator<<(std::ostream& os, const setset& ss);
};

void setset::add(const set& s) {
    if (!contains(s)) {
        sets.push_back(s);
    }
}

bool setset::contains(const set& s) const {
    for ( auto i : sets) {
        if (i == s) {
            return true;
        }
    }
    return false;
}
std::ostream& operator<<(std::ostream& os, const setset& ss) {
    os << "{" << std::endl;
    bool started = false;
    for (auto i : ss.sets) {
        if (started) {
            os << "," << std::endl;
        }
        started = true;
        os << i;
    }
    os << std::endl << "}" << std::endl;
    return os;
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
