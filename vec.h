#ifndef vec_h
#define vec_h

#include "rat.h"

#define DEFINE_VEC_INPLACE_POINTWISE(op)         \
    inline vec& operator op(const vec& other) {  \
        for (int i = 0; i < N; ++i) {            \
            (*this)[i] op other[i];              \
        }                                        \
        return *this;                            \
    } 

#define DEFINE_VEC_INPLACE_SCALAR(op)           \
    inline vec& operator op(rat n) {            \
        for (int i = 0; i < N; ++i) {           \
            (*this)[i] op n;                    \
        }                                       \
        return *this;                           \
    }                                           

#define DEFINE_VEC_BINARY_POINTWISE(op)                     \
    template<int N>                                         \
    inline vec<N> operator op(vec<N> a, const vec<N>& b) {  \
        a op##= b;                                          \
        return a;                                           \
    } 

#define DEFINE_VEC_BINARY_SCALAR(op)                        \
    template<int N>                                         \
    inline vec<N> operator op(vec<N> a, rat b) {            \
        a op##= b;                                          \
        return a;                                           \
    } 



template<int N>
class vec {
public:
    rat data[N];

    inline rat operator[](int i) const {
        return data[i];
    }

    inline rat& operator[](int i) {
        return data[i];
    }

    DEFINE_VEC_INPLACE_POINTWISE(+=)
    DEFINE_VEC_INPLACE_POINTWISE(-=)
    DEFINE_VEC_INPLACE_POINTWISE(*=)
    DEFINE_VEC_INPLACE_POINTWISE(/=)

    DEFINE_VEC_INPLACE_SCALAR(+=)
    DEFINE_VEC_INPLACE_SCALAR(-=)
    DEFINE_VEC_INPLACE_SCALAR(*=)
    DEFINE_VEC_INPLACE_SCALAR(/=)

    inline rat dot(const vec& other) const {
        vec prod = *this * other;
        rat result;
        for (int i = 0; i < N; ++i) {
            result += prod[i];

        }
        return result;
    }

    inline rat sqmag() const {
        return dot(*this);
    }



    
#define VEC_DEFINE_MAP(before, ii, body, after) \
    before;                                     \
    for (int ii = 0; ii < N; ++ii) {            \
        body;                                   \
    }                                           \
    after;                                              
#define VEC_DEFINE_FOREACH_METHOD(methodName) \
    VEC_DEFINE_MAP(, i, (*this)[i].methodName(),)
    
#define VEC_DEFINE_AND_METHOD(name)             \
    VEC_DEFINE_MAP(, i, if (! (*this)[i].name()) { return false; }, return true;)
#define VEC_DEFINE_OR_METHOD(name)              \
    VEC_DEFINE_MAP(, i, if ((*this)[i].name()) { return true; }, return false;)
    

    inline void reduce() {
        VEC_DEFINE_FOREACH_METHOD(reduce);
    }

    inline bool is_zero() const {
        VEC_DEFINE_AND_METHOD(is_zero);
    }

    inline bool has_NaN() const {
        VEC_DEFINE_OR_METHOD(is_NaN);
    }
    inline bool has_infty() const {
        VEC_DEFINE_OR_METHOD(is_infty);
    }

    inline vec proj_onto(const vec& other) {
        rat d = dot(other);
        rat sqm = other.sqmag();
        vec result = (d/sqm) * other;
        return result;
    }

};
template <int N>
inline vec<N> operator*(rat n, vec<N> v) {
    v *= n;
    return v;
}

template<int N>
std::ostream& operator<<(std::ostream& os, const vec<N>& v) {
    os << "(";

    bool started = false;
    for (int i = 0; i < N; ++i) {
        if (started) {
            os << ", ";
        }
        started = true;

        os << v[i];
    }
    os << ")";

    return os;
}


template<int N>
inline bool operator==(const vec<N>& lhs, const vec<N>& rhs){
    for (int i = 0; i < N; ++i) {
        if (lhs[i] != rhs[i]) {
            return false;
        }
    }
    return true;
}

template<int N>
inline bool operator!=(const vec<N>& lhs, const vec<N>& rhs){ return !operator==(lhs,rhs); }

DEFINE_VEC_BINARY_POINTWISE(+)
DEFINE_VEC_BINARY_POINTWISE(-)
DEFINE_VEC_BINARY_POINTWISE(*)


DEFINE_VEC_BINARY_SCALAR(+)
DEFINE_VEC_BINARY_SCALAR(-)
DEFINE_VEC_BINARY_SCALAR(*)

template <int N>
inline vec<N> operator-(const vec<N>& v) {
    vec<N> result = v;
    for (int i = 0; i < N; ++i) {
        result[i] = -result[i];
    }
    return result;
}

#endif  // vec_h
