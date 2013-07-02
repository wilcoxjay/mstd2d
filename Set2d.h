#include <iostream>
#include <cassert>
#include <cstring>

/**
   Dense representation of 2d sets of integers.

   Useful when space is not an issue, and its all about speed.
 */
template <unsigned int X, unsigned int Y> 
class Set2d {
protected:
    char m_data[X*Y];

    inline static bool in_bounds(int i, int j) {
        return 0 <= i && i < (int)X && 0 <= j && j < (int)Y;
    }

    inline static int linear_index(int i, int j) {
        return i*Y + j;
    }

public:
    inline char* unsafe_get_data() {
        return m_data;
    }

    inline bool contains(int i, int j) const {
        assert(in_bounds(i, j));
        return m_data[linear_index(i,j)] != 0;
    }

    inline void add(int i, int j) {
        assert(in_bounds(i, j));
        m_data[linear_index(i,j)] = true;
    }

    inline void remove(int i, int j) {
        assert(in_bounds(i, j));
        m_data[linear_index(i,j)] = false;
    }

    inline void clear() {
        memset(m_data, 0, X*Y);
    }
    
    void print() const {
        std::cout << "{";
        bool started = false;
        for (unsigned int i = 0; i < X; ++i) {
            for (unsigned int j = 0; j < Y; ++j) {
                if (contains(i,j)) {
                    if (started) {
                        std::cout << ", ";
                    }
                    started = true;
                    std::cout << "(" << i << "," << j << ")";
                }
            }
        }
        std::cout << "}" << std::endl;
    }
};
