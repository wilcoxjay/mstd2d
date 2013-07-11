#ifndef setset_h
#define setset_h

#include "set.h"
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
    for (auto i : sets) {
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
#endif  // setset_h
