#ifndef SPARSE_SET_H
#define SPARSE_SET_H
#include <G3D/Array.h>
#include <iostream>

class SparseSet2d {
public:
    struct Point2 {
        char x;
        char y;
        //mutable bool essential;
        Point2() /*: essential(false)*/ { }
        Point2(char _x, char _y) : x(_x), y(_y)  /*, essential(false)*/ { }
    };

    SparseSet2d() : m_mutated(true) { }

    inline void add(char i, char j) {
        m_mutated = true;
        data.append(Point2(i, j));
    }

    inline void clear() {
        m_mutated = true;
        data.resize(0, false);
    }

    inline const Point2* getPoints() const {
        return data.getCArray();
    }

    inline int size() const {
        return data.size();
    }

    // inline void remove(int i) {
    //     m_mutated = true;
    //     data.remove(i);
    // }

    inline unsigned int hash() const {
        if (!m_mutated) { return m_hash; }
        m_mutated = false;
        char *key = (char*) getPoints();
        size_t len = size() * sizeof(Point2);
        uint32_t hash, i;
        for(hash = i = 0; i < len; ++i) {
            hash += key[i];
            hash += (hash << 10);
            hash ^= (hash >> 6);
        }
        hash += (hash << 3);
        hash ^= (hash >> 11);
        hash += (hash << 15);
        m_hash = hash;
        return hash;
    }

private:
    mutable bool m_mutated;
    mutable unsigned int m_hash;
    G3D::Array<Point2> data;
};

inline std::ostream& operator<<(std::ostream& os, const SparseSet2d& set) {
    os << "{";
    bool started = false;
    for (int i = 0; i < set.size(); ++i) {
        if (started) {
            os << ", ";
        }
        started = true;
        os << "(" << ((int)set.getPoints()[i].x) << ","
           << ((int)set.getPoints()[i].y) << ")";
    }
    os << "}";
    return os;
}

inline bool operator==(const SparseSet2d::Point2& a, const SparseSet2d::Point2& b) {
    return a.x == b.x && a.y == b.y;
}
inline bool operator!=(const SparseSet2d::Point2& a, const SparseSet2d::Point2& b) {
    return !(a == b);
}

inline bool operator==(const SparseSet2d& a, const SparseSet2d& b) {
    if (a.size() != b.size()) {
        return false;
    }
    if (a.hash() != b.hash()) {
        return false;
    }
    const SparseSet2d::Point2* aData = a.getPoints();
    const SparseSet2d::Point2* bData = b.getPoints();

    for (int i = 0; i < a.size(); ++i) {
        if (aData[i] != bData[i]) {
            return false;
        }
    }
    return true;    
}
inline bool operator!=(const SparseSet2d& a, const SparseSet2d& b) {
    return !(a == b);
}


#endif  // SPARSE_SET_H
