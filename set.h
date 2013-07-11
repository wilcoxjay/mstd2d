#ifndef set_h
#define set_h

#define DIM 8

#include "vec.h"


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
#endif  // set_h
