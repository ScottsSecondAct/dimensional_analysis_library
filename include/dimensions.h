#pragma once
#include <cmath>
#include <compare>
#include <concepts>
#include <ostream>
#include <string>

template <int M, int L, int T, int I=0, int K=0, int N=0, int J=0>
struct Dimensions {
    static constexpr int mass       = M;
    static constexpr int length     = L;
    static constexpr int time       = T;
    static constexpr int current    = I;
    static constexpr int temp       = K;
    static constexpr int amount     = N;
    static constexpr int luminosity = J;
};

template <typename D1, typename D2>
struct DimAdd {
    using type = Dimensions<
        D1::mass    + D2::mass,    D1::length + D2::length, D1::time   + D2::time,
        D1::current + D2::current, D1::temp   + D2::temp,
        D1::amount  + D2::amount,  D1::luminosity + D2::luminosity>;
};

template <typename D1, typename D2>
struct DimSub {
    using type = Dimensions<
        D1::mass    - D2::mass,    D1::length - D2::length, D1::time   - D2::time,
        D1::current - D2::current, D1::temp   - D2::temp,
        D1::amount  - D2::amount,  D1::luminosity - D2::luminosity>;
};

// Multiply all exponents by N — used by pow<N>
template<typename D, int N>
struct DimScale {
    using type = Dimensions<D::mass*N, D::length*N, D::time*N,
                            D::current*N, D::temp*N, D::amount*N, D::luminosity*N>;
};

// Halve all exponents — used by sqrt; requires all exponents to be even
template<typename D>
struct DimHalve {
    static_assert(D::mass       % 2 == 0, "sqrt: mass exponent must be even");
    static_assert(D::length     % 2 == 0, "sqrt: length exponent must be even");
    static_assert(D::time       % 2 == 0, "sqrt: time exponent must be even");
    static_assert(D::current    % 2 == 0, "sqrt: current exponent must be even");
    static_assert(D::temp       % 2 == 0, "sqrt: temp exponent must be even");
    static_assert(D::amount     % 2 == 0, "sqrt: amount exponent must be even");
    static_assert(D::luminosity % 2 == 0, "sqrt: luminosity exponent must be even");
    using type = Dimensions<D::mass/2, D::length/2, D::time/2,
                            D::current/2, D::temp/2, D::amount/2, D::luminosity/2>;
};

// Forward declaration needed for IsQuantity concept
template <typename Dim>
struct Quantity;

template <typename T>
concept IsQuantity = requires {
    typename T::DimensionType;
} && std::is_same_v<T, Quantity<typename T::DimensionType>>;

template <typename Dim>
struct Quantity {
    using DimensionType = Dim;
    double value;
    explicit constexpr Quantity(double v) : value(v) {}

    // Quantity * Quantity → DimAdd
    template <IsQuantity RHS>
    constexpr auto operator*(RHS rhs) const {
        return Quantity<typename DimAdd<Dim, typename RHS::DimensionType>::type>(value * rhs.value);
    }

    // Quantity / Quantity → DimSub
    template <IsQuantity RHS>
    constexpr auto operator/(RHS rhs) const {
        return Quantity<typename DimSub<Dim, typename RHS::DimensionType>::type>(value / rhs.value);
    }

    // Same-dimension addition
    constexpr Quantity operator+(Quantity rhs) const { return Quantity(value + rhs.value); }

    // Same-dimension subtraction
    constexpr Quantity operator-(Quantity rhs) const { return Quantity(value - rhs.value); }

    // Unary negation
    constexpr Quantity operator-() const { return Quantity(-value); }

    // Scalar multiplication
    constexpr Quantity operator*(double s) const { return Quantity(value * s); }
    friend constexpr Quantity operator*(double s, Quantity q) { return Quantity(s * q.value); }

    // Scalar division
    constexpr Quantity operator/(double s) const { return Quantity(value / s); }

    // Spaceship comparison (requires same dimension — enforced by type system)
    constexpr auto operator<=>(const Quantity&) const = default;
};

// =============================================================================
// Math free functions
// =============================================================================

// pow<N>(q) — raise a Quantity to an integer power; scales all dimension exponents by N
template<int N, IsQuantity Q>
constexpr auto pow(Q q) {
    return Quantity<typename DimScale<typename Q::DimensionType, N>::type>(std::pow(q.value, N));
}

// sqrt(q) — square root; requires all dimension exponents to be even (checked at compile time)
template<IsQuantity Q>
constexpr auto sqrt(Q q) {
    return Quantity<typename DimHalve<typename Q::DimensionType>::type>(std::sqrt(q.value));
}

// abs(q) — absolute value; preserves dimension
template<IsQuantity Q>
constexpr Q abs(Q q) { return Q(std::abs(q.value)); }

// =============================================================================
// Stream output  (e.g.  "9.81 [m·s^-2]")
// =============================================================================

namespace detail {
    template<typename D>
    inline std::string dim_string() {
        const char* names[7] = {"kg", "m", "s", "A", "K", "mol", "cd"};
        const int   exps[7]  = {D::mass, D::length, D::time, D::current,
                                 D::temp, D::amount, D::luminosity};
        std::string result;
        for (int i = 0; i < 7; ++i) {
            if (exps[i] == 0) continue;
            if (!result.empty()) result += "\xc2\xb7"; // UTF-8 middle dot ·
            result += names[i];
            if (exps[i] != 1) { result += '^'; result += std::to_string(exps[i]); }
        }
        return result.empty() ? "1" : result;
    }
}

template<IsQuantity Q>
std::ostream& operator<<(std::ostream& os, Q q) {
    return os << q.value << " [" << detail::dim_string<typename Q::DimensionType>() << "]";
}
