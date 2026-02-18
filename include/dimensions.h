#pragma once
#include <concepts>

template <int M, int L, int T, int I=0, int K=0, int N=0, int J=0>
struct Dimensions {
    static constexpr int mass = M; 
    static constexpr int length = L; 
    static constexpr int time = T;
};

template <typename D1, typename D2>
struct DimAdd {
    using type = Dimensions<D1::mass + D2::mass, D1::length + D2::length, D1::time + D2::time>;
};

template <typename Dim>
struct Quantity {
    using DimensionType = Dim;
    double value;
    explicit constexpr Quantity(double v) : value(v) {}

    template <typename OtherDim>
    constexpr auto operator*(Quantity<OtherDim> rhs) const {
        return Quantity<typename DimAdd<Dim, OtherDim>::type>(value * rhs.value);
    }
};

using Length = Quantity<Dimensions<0, 1, 0>>;
using Time = Quantity<Dimensions<0, 0, 1>>;
using Velocity = Quantity<Dimensions<0, 1, -1>>;

constexpr Length operator "" _m(long double d) { return Length(static_cast<double>(d)); }
constexpr Time operator "" _s(long double d) { return Time(static_cast<double>(d)); }