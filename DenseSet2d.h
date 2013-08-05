#ifndef DENSE_SET_H
#define DENSE_SET_H
#include <iostream>
#include <cassert>
#include <cstring>

/**
   Dense representation of 2d sets of integers.
 */
class DenseSet2d {
protected:
    const int m_x;
    const int m_y;
    char* m_data;

    inline bool in_bounds(int i, int j) const {
        return 0 <= i && i < (int)x() && 0 <= j && j < (int)y();
    }

    inline int linear_index(int i, int j) const {
        return i*y() + j;
    }

public:
    DenseSet2d(int x, int y) : m_x(x), m_y(y), m_data(new char[x*y]) { }
    ~DenseSet2d() {
        delete[] m_data;
    }

    inline int x() const {
        return m_x;
    }

    inline int y() const {
        return m_y;
    }

    inline char* unsafe_get_data() {
        return m_data;
    }

    inline bool contains(char i, char j) const {
        assert(in_bounds(i, j));
        return m_data[linear_index(i,j)] != 0;
    }

    inline void add(char i, char j) {
        assert(in_bounds(i, j));
        m_data[linear_index(i,j)] = true;
    }

    inline void remove(char i, char j) {
        assert(in_bounds(i, j));
        m_data[linear_index(i,j)] = false;
    }

    inline void clear() {
        memset(m_data, 0, x()*y());
    }
    
};

std::ostream& operator<<(std::ostream& os, const DenseSet2d& set) {
    os << "{";
    bool started = false;
    for (int i = 0; i < set.x(); ++i) {
        for (int j = 0; j < set.y(); ++j) {
            if (set.contains(i,j)) {
                if (started) {
                    os << ", ";
                }
                started = true;
                os << "(" << i << "," << j << ")";
            }
        }
    }
    os << "}";
    return os;
}
#endif  // DENSE_SET_H
