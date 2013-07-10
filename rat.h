// -*- c++ -*-
#ifndef rat_h
#define rat_h

#include <iostream>
#include <cstdlib>

long long min(long long a, long long b) { return a > b ? b : a; }

class rat {
public:
    long long num;
    long long denom;  // note: always >= 0

    rat() : num(0), denom(1) { }
    rat(long long _n, long long _d) : num(_n), denom(_d) { }
    explicit rat(long long x) : num(x), denom(1) { }

    inline rat& operator-=(const rat& other) {
        num *= other.denom;
        num -= other.num * denom;
        denom *= other.denom;
        reduce();
        return *this;
    }
    inline rat& operator*=(const rat& other) {
        num *= other.num;
        denom *= other.denom;
        reduce();
        return *this;
    }
    inline rat& operator/=(const rat& other) {
        num *= other.denom;
        denom *= other.num;
        reduce();
        return *this;
    }

    inline long long gcd(long long a, long long b) {
        while (b != 0) {
            long long t = b;
            b = a % b;
            a = t;
        }
        return a;
    }

    inline void reduce();


    inline bool is_zero() const {
        return num == 0 && denom != 0;
    }
    inline bool is_NaN() const {
        return num == 0 && denom == 0;
    }
    inline bool is_infty() const {
        return num != 0 && denom == 0;
    }
};


inline bool operator==(const rat& lhs, const rat& rhs) { return rhs.denom * lhs.num == lhs.denom * rhs.num; }

inline bool operator!=(const rat& lhs, const rat& rhs) { return !operator==(lhs,rhs); }

std::ostream& operator<<(std::ostream& os, rat r) {
    if (r.num == 0) {
        if (r.denom == 0) {
            os << "NaN";
        } else {
            os << "0";
        }
    } else if (r.denom == 0) {
        os << "infty";
    } else if (r.denom == 1) {
        os << r.num;
    } else {
        os << r.num << "/" << r.denom;
    }
    return os;
}

inline rat& operator+=(rat& me, const rat& other) {
    me.num *= other.denom;
    me.num += other.num * me.denom;
    me.denom *= other.denom;
    me.reduce();
    return me;
}

inline rat operator+(rat a, const rat& b) {
    a += b;
    return a;
}
inline rat operator-(rat a, const rat& b) {
    a -= b;
    return a;
}
inline rat operator*(rat a, const rat& b) {
    a *= b;
    return a;
}
inline rat operator/(rat a, const rat& b) {
    a /= b;
    return a;
}


inline void rat::reduce() {
    if (num == 0) {
        if (denom > 0) {
            denom = 1;
        }
        return;
    }
    if (denom == 0) {
        if (num != 0) {
            num = 1;
        }
        return;
    }

    int sign = num > 0 ? 1 : num < 0 ? -1 : 0;
    num = llabs(num);

    long long d = gcd(num, denom);
    num /= d;
    denom /= d;

    num *= sign;
}


#endif  // rat_h
