# Dimensional Analysis Library

![AI Assisted](https://img.shields.io/badge/AI%20Assisted-Claude-blue?logo=anthropic)

A header-only, zero-overhead C++20 library for compile-time dimensional analysis. Physical units are tracked as type-level information — `Length / Time` automatically yields `Velocity`, and passing a speed where a mass is expected is a compile error, not a runtime bug.

## Why This Project

Unit errors are among the most expensive bugs in numerical software. NASA's Mars Climate Orbiter was lost to a pound-force / newton mismatch. Physiological drug-dosing errors frequently stem from mg/kg vs mg/L confusion. These failures share a root cause: the type system of the language used didn't know that a `double` labelled "force" and a `double` labelled "impulse" are physically different things.

The goal here was to solve this at the type-system level using modern C++ metaprogramming — making unit mismatches compile errors with zero runtime cost. The library encodes all seven SI base dimension exponents directly into the type using a non-type template parameter pack, performs arithmetic on those exponents at compile time, and wraps a single `double` in the result. The abstraction costs exactly nothing: `sizeof(Quantity<anything>) == sizeof(double)`.

This was also a deliberate exercise in C++20 library design: concepts to constrain operators, user-defined literals for ergonomic syntax, `inline namespace` for opt-in scoping, and `constexpr`/`inline` throughout for zero-cost abstraction guarantees.

## Quick Look

```cpp
#include "units.h"

// Compile error — arguments are swapped, caught before runtime
Energy kinetic_energy(Mass m, Velocity v) { return 0.5 * m * v * v; }
kinetic_energy(11200.0_m/_s, 70.0_kg);   // error: Velocity passed where Mass expected
kinetic_energy(70.0_kg, 11200.0_m/_s);   // correct: KE ≈ 4.39 GJ (Earth escape speed)

// Unit conversions happen at construction — all stored internally as SI
Velocity runway_speed = 160.0_kn;    // stored as 82.3 m/s
Length  field_length  = 300_yd;      // stored as 274.3 m
Force   brake_force   = 12000_lbf;   // stored as 53.4 kN

// Math operations carry dimension through automatically
auto stopping_dist = brake_force / (70.0_kg * 9.81_m / (1.0_s * 1.0_s));
static_assert(std::is_same_v<decltype(stopping_dist), Length>);

// Physical constants are typed — can't mix them up
Energy photon = constants::h * Frequency(4.7e14);  // E = hf, blue light ≈ 3.1e-19 J
Energy rest   = constants::m_e * pow<2>(constants::c);  // E = mc², electron ≈ 8.2e-14 J

// Debug output includes the dimension
std::cout << 9.81_m / (1.0_s * 1.0_s);  // "9.81 [m·s^-2]"
```

## Features

### Type Aliases

| Category | Types |
|---|---|
| SI base | `Mass` `Length` `Time` `Current` `Temperature` `Amount` `Luminosity` |
| Mechanics | `Area` `Volume` `Velocity` `Acceleration` `Momentum` `Force` `Energy` `Power` `Pressure` `Frequency` `Density` `DynamicViscosity` `KinematicViscosity` `Action` |
| Electromagnetism | `Charge` `Voltage` `Resistance` `Capacitance` `Inductance` `MagneticFlux` `MagneticField` `Conductance` `ElectricField` |
| Thermodynamics | `Entropy` `SpecificHeat` `ThermalConductivity` |
| Chemistry | `MolarMass` `Concentration` `CatalyticActivity` `MolarEnergy` `MolarEntropy` |
| Radiation | `RadioactiveActivity` `AbsorbedDose` |
| Photometry | `LuminousFlux` `Illuminance` |

### Math Operations

All arithmetic returns a correctly-dimensioned type — no casts, no `.value` access needed until a raw number is required.

```cpp
auto area    = 4.0_m * 5.0_m;              // Area(20.0)
auto speed   = 120.0_km / 1.0_hr;          // Velocity(33.3 m/s)
auto power   = 100.0_J / 5.0_s;            // Power(20.0 W)
auto e_field = 12.0_V / 3.0_mm;            // ElectricField(4000 V/m)

auto vol    = pow<3>(2.0_m);               // Volume(8.0) — scales all exponents by 3
auto side   = sqrt(16.0_m * 16.0_m);       // Length(16.0) — compile check: all exps even
auto escape = sqrt(2.0 * constants::G.value * 5.97e24 / 6.37e6);  // ≈ 11.2 km/s

auto dist   = abs(-400.0_m);               // Length(400.0)
```

### Physical Constants (`namespace constants`)

All constants carry their correct type. Using `k_B` (Entropy, J/K) where Energy is expected is a compile error.

| Constant | Value | Type |
|---|---|---|
| `c` | 299 792 458 m/s | `Velocity` |
| `h` | 6.626 070 15 × 10⁻³⁴ J·s | `Action` |
| `hbar` | 1.054 571 817 × 10⁻³⁴ J·s | `Action` |
| `e` | 1.602 176 634 × 10⁻¹⁹ C | `Charge` |
| `k_B` | 1.380 649 × 10⁻²³ J/K | `Entropy` |
| `R` | 8.314 462 618 J/(mol·K) | `MolarEntropy` |
| `m_e` | 9.109 383 7015 × 10⁻³¹ kg | `Mass` |
| `m_p` | 1.672 621 923 69 × 10⁻²⁷ kg | `Mass` |
| `m_n` | 1.674 927 498 04 × 10⁻²⁷ kg | `Mass` |
| `G` | 6.674 30 × 10⁻¹¹ m³/(kg·s²) | *(local alias)* |
| `N_A` | 6.022 140 76 × 10²³ /mol | *(local alias)* |
| `sigma` | 5.670 374 419 × 10⁻⁸ W/(m²·K⁴) | *(local alias)* |

`c`, `h`, `e`, `k_B`, `R`, `N_A` are exact values from the 2019 SI redefinition. `m_e`, `m_p`, `m_n`, `G`, `sigma` are CODATA 2018 measured values.

### User-Defined Literals

Every UDL accepts both `long double` (`1.5_kg`) and `unsigned long long` (`2_kg`). All live in `inline namespace si_literals`, available by default.

| Category | UDLs |
|---|---|
| Mass | `_kg` `_g` `_mg` `_Da` `_u` `_tonne` `_lb` `_lbm` `_oz` `_slug` |
| Length | `_m` `_km` `_cm` `_mm` `_in` `_ft` `_yd` `_mi` `_nmi` `_au` `_ly` `_pc` `_kpc` `_Mpc` |
| Time | `_s` `_ms` `_us` `_min` `_hr` `_day` `_yr` |
| Energy | `_J` `_kJ` `_eV` `_meV` `_MeV` `_GeV` `_cal` `_kcal` `_Wh` `_kWh` `_BTU` |
| Voltage | `_MV` `_kV` `_V` `_mV` `_uV` |
| Current | `_A` `_mA` `_uA` `_nA` |
| Charge | `_C` `_mC` `_uC` `_nC` `_pC` |
| Resistance | `_Mohm` `_kohm` `_ohm` `_mohm` |
| Capacitance | `_F` `_mF` `_uF` `_nF` `_pF` |
| Inductance | `_H` `_mH` `_uH` `_nH` |
| Pressure | `_Pa` `_kPa` `_MPa` `_bar` `_atm` `_psi` `_torr` `_mmHg` |
| Power | `_W` `_kW` `_MW` `_hp` |
| Frequency | `_Hz` `_kHz` `_MHz` `_GHz` |
| Other | `_N` `_kN` `_lbf` · `_K` `_degC` `_degF` · `_mol` `_mmol` · `_L` `_mL` · `_Wb` `_T` `_S` · `_kn` `_b` · `_Bq` `_Ci` `_Gy` `_Sv` · `_lm` `_lx` · `_cd` |

## Architecture

The library is two independent headers. Include only `units.h` — it pulls in the core engine automatically.

```
your_code.cpp
      │
      │  #include "units.h"
      ▼
┌───────────────────────────────────────────────────────────────┐
│  units.h  (user-facing — include only this one)               │
│                                                               │
│  Type aliases    Mass, Velocity, Force, Capacitance, ...      │
│  namespace constants   c, h, k_B, m_e, G, N_A, sigma, ...    │
│  inline namespace si_literals                                  │
│      UDLs: _kg _m _s _J _V _uF _GHz _atm _ly ...             │
│      Both long double and unsigned long long overloads         │
└───────────────────────────┬───────────────────────────────────┘
                            │  #include "dimensions.h"
                            ▼
┌───────────────────────────────────────────────────────────────┐
│  dimensions.h  (core engine)                                  │
│                                                               │
│  Dimensions<M,L,T,I,K,N,J>   7 int template parameters       │
│                               (mass, length, time, current,   │
│                                temp, amount, luminosity)       │
│                                                               │
│  DimAdd<D1,D2>   exponent-wise addition   (used by * )        │
│  DimSub<D1,D2>   exponent-wise subtraction (used by / )       │
│  DimScale<D,N>   multiply all exponents by N (used by pow<N>) │
│  DimHalve<D>     divide all exponents by 2 (used by sqrt)     │
│                  static_assert: all exponents must be even     │
│                                                               │
│  IsQuantity      C++20 concept                                │
│  Quantity<Dim>   wraps one double, all operators constexpr    │
│                  sizeof == 8; layout identical to double       │
│                                                               │
│  pow<N>(q)   sqrt(q)   abs(q)   operator<<                    │
└───────────────────────────────────────────────────────────────┘

  ecs.h  (independent — no dependency on the above)
  TypeRegistry      compile-time unique integer IDs per type
  ComponentPool<T>  sparse set: O(1) add/lookup, cache-friendly iteration
  Registry          owns pools; view<A,B,...>(func) iterates smallest pool
```

## Technical Highlights

### Compile-Time Dimension Arithmetic

All seven SI base dimension exponents are stored as non-type template parameters:

```cpp
template <int M, int L, int T, int I=0, int K=0, int N=0, int J=0>
struct Dimensions {
    static constexpr int mass = M, length = L, time = T, /* ... */;
};
```

Multiplication and division map to exponent addition and subtraction on those integers:

```cpp
// Force {1,1,-2} × Length {0,1,0} = Energy {1,2,-2}
template <typename D1, typename D2>
struct DimAdd {
    using type = Dimensions<
        D1::mass + D2::mass, D1::length + D2::length, D1::time + D2::time, /* ... */>;
};
```

The compiler evaluates all of this at template-instantiation time. The resulting binary contains no dimension bookkeeping — only the `double` arithmetic.

### Zero-Overhead Guarantee

`Quantity<Dim>` has exactly one data member: `double value`. Every operator is `constexpr` and `inline`. The compiler sees through the wrapper and generates machine code identical to hand-written `double` arithmetic:

```cpp
// These compile to identical assembly
double  raw_ke(double m, double v)   { return 0.5 * m * v * v; }
Energy typed_ke(Mass m, Velocity v)  { return 0.5 * m * v * v; }
```

### `sqrt` — Compile-Time Even-Exponent Check

`sqrt` is only physically meaningful when all exponents are even — otherwise the result would require fractional exponents, which the `int` pack cannot represent. `DimHalve` enforces this with static asserts that fire before the binary is built:

```cpp
template<typename D>
struct DimHalve {
    static_assert(D::mass   % 2 == 0, "sqrt: mass exponent must be even");
    static_assert(D::length % 2 == 0, "sqrt: length exponent must be even");
    // ... all 7 slots
    using type = Dimensions<D::mass/2, D::length/2, /* ... */>;
};

sqrt(16.0_m * 16.0_m)  // Area → Length ✓
sqrt(Velocity{1.0})     // compile error: time exponent -1 is odd
```

This is also the fundamental limit of the `int`-exponent design. Handling arbitrary `sqrt` would require replacing `int` with `std::ratio` — a foundational redesign listed in the roadmap.

### `IsQuantity` C++20 Concept

The `*` and `/` operators are constrained by a concept rather than SFINAE, producing clear diagnostic messages:

```cpp
template <typename T>
concept IsQuantity = requires { typename T::DimensionType; }
                  && std::is_same_v<T, Quantity<typename T::DimensionType>>;

// Somewhere in user code:
auto bad = 5.0_m * 3.0;   // fine — scalar overload
auto bad = 5.0_m * 3;     // error: "constraint not satisfied: IsQuantity"
                           // not: 40 lines of substitution failure
```

### UDL Design — Dual Overloads and Inline Namespace

Every UDL provides both `long double` and `unsigned long long` overloads so integer literals work naturally:

```cpp
constexpr Mass operator""_kg(long double v)       { return Mass(static_cast<double>(v)); }
constexpr Mass operator""_kg(unsigned long long v) { return Mass(static_cast<double>(v)); }

auto a = 1.5_kg;  // long double path
auto b = 2_kg;    // unsigned long long path — no trailing .0 required
```

All UDLs live inside `inline namespace si_literals`. The `inline` keyword makes them available in the enclosing namespace (global scope) by default, preserving backward compatibility, while also allowing explicit opt-in:

```cpp
using namespace si_literals;  // explicit — useful to signal intent in headers
auto x = 5_m;                 // also works with no using declaration
```

Temperature UDLs convert from the affine scale to Kelvin at construction — the library tracks dimensions, not offset scales:

```cpp
Temperature body  = 37_degC;   // stores 310.15 K
Temperature water = 212_degF;  // stores 373.15 K
Temperature abs_z = 0_K;       // stores 0.0 K
```

### ECS Sparse Set (`ecs.h`)

Independent of dimensional analysis, `ecs.h` implements a sparse-set Entity Component System in under 80 lines:

- `ComponentPool<T>` maintains a `sparse` index vector mapping entity IDs to positions in a `dense` packed array. Add and lookup are O(1); iteration is cache-friendly since all components of a type are contiguous.
- `Registry::view<A, B>(func)` iterates the smallest requested pool and calls `func` only for entities present in all pools. No allocations, no temporaries.

```cpp
Registry reg;
reg.get_pool<Position>().assign(0, {1.0f, 2.0f});
reg.get_pool<Velocity>().assign(0, {0.5f, 0.0f});
reg.get_pool<Velocity>().assign(1, {1.0f, 0.0f});  // entity 1: no Position

reg.view<Position, Velocity>([](Position& p, Velocity& v) {
    p.x += v.x;  // called only for entity 0
});
```

## Build & Run

### Prerequisites

- Clang 18.1.3+ (required — GCC support untested; library relies on Clang's C++20 concept diagnostics)
- CMake 3.20+
- Ninja
- Internet on first configure (GoogleTest fetched via `FetchContent`)

### Build

```bash
export CXX=clang++-18
export CC=clang-18

cmake -G Ninja -B build -S .
cmake --build build -j$(nproc)

./build/engine_tests   # 117 tests, 16 suites, all pass
./build/engine_demo    # exercises dimensional arithmetic + ECS
```

There is no install step. This is a header-only library — copy `include/dimensions.h` and `include/units.h` (and optionally `include/ecs.h`) into your project.

### Using in Your Project

```cpp
#include "units.h"   // pulls in dimensions.h automatically

// E = mc²
Energy rest_mass_energy = constants::m_e * pow<2>(constants::c);
std::cout << rest_mass_energy;   // "8.18711e-14 [kg·m^2·s^-2]"

// RC time constant — result type is checked to be Time
Time tau = 10.0_kohm * 100.0_uF;   // τ = 1 ms
```

## Development Process & AI Collaboration

This project was built incrementally using AI assistance (Anthropic's Claude) as a collaborator for architecture design, metaprogramming patterns, and debugging — the same way a professional engineer uses documentation, a senior colleague, or reference implementations.

Specific contributions:
- **Architecture**: The 7-slot `Dimensions` pack, `DimAdd`/`DimSub` trait design, and `IsQuantity` concept were designed collaboratively, evaluating trade-offs against alternatives (e.g. `std::ratio` for fractional exponents, tag-based vs. pack-based dimension encoding).
- **Debugging**: Clang concept error diagnostics and `constexpr` evaluation failures were diagnosed together from compiler output.
- **Coverage**: The 117-test suite was designed to cover dimensional correctness, type propagation, UDL conversion accuracy (cross-checked against NIST/BIPM/IAU), physical constant values (2019 SI redefinition exact values + CODATA 2018), and composite physics formulas.

Every design decision was understood and evaluated by hand before integration. The AI accelerated implementation and surfaced subtle correctness issues — for example, the even-exponent requirement for `sqrt`, and why `inline namespace` rather than a plain namespace is the correct choice for backward-compatible UDL opt-in. The learning is in understanding *why* `DimHalve` needs static asserts and *why* `Quantity` must be exactly 8 bytes, not in writing the boilerplate.

## Skills Demonstrated

- **Template metaprogramming**: Non-type template parameters, recursive trait structs (`DimAdd`, `DimScale`, `DimHalve`), SFINAE-free constraint enforcement via C++20 concepts
- **Zero-overhead abstraction**: `constexpr`/`inline` throughout, layout verification, compiler-transparent wrapper design
- **C++20 features**: Concepts, defaulted `operator<=>` (enables all six comparisons), `inline constexpr`, `inline namespace`
- **Library API design**: Dual UDL overloads (float + integer), `inline namespace si_literals` for opt-in scoping, `namespace constants` for typed physical constants with exact values
- **Physics accuracy**: 2019 SI redefinition exact values for `c`, `h`, `e`, `k_B`; CODATA 2018 for measured constants; NIST/IAU/BIPM values for unit conversions
- **Testing**: 117-test GoogleTest suite covering dimension arithmetic, type propagation, UDL conversion accuracy, physical constants, and composite physics formulas
- **Build systems**: CMake with `FetchContent`, Ninja, compiler environment configuration

## Roadmap

- [x] 7-slot SI dimension engine (`Dimensions`, `DimAdd`, `DimSub`)
- [x] `Quantity<Dim>` — 8-byte zero-overhead wrapper, all operators `constexpr`
- [x] `IsQuantity` C++20 concept
- [x] `operator<=>` — all six comparison operators on same-dimension quantities
- [x] `pow<N>`, `sqrt` (even-exponent compile-time check), `abs`
- [x] `operator<<` — prints value with dimension string (`9.81 [m·s^-2]`)
- [x] 50+ type aliases — mechanics, EM, thermo, chemistry, radiation, photometry
- [x] 12 physical constants in `namespace constants` (2019 SI exact + CODATA 2018)
- [x] 140+ UDLs with both `long double` and `unsigned long long` overloads
- [x] `inline namespace si_literals` — backward-compatible scoped opt-in
- [x] ECS sparse-set (`TypeRegistry`, `ComponentPool<T>`, `Registry::view`)
- [x] 117 tests, 16 suites, all passing
- [ ] Fractional exponents via `std::ratio` — enables `sqrt` on arbitrary dimensions; requires foundational redesign of the exponent pack
- [ ] Strongly-typed `Angle` — dimensionless but a common bug source; needs tag-type approach outside the SI dimension model
- [ ] `std::numeric_limits<Quantity<D>>` specialization for standard algorithm compatibility
- [ ] `clamp`, `min`, `max` free functions

## License

MIT License — Copyright (c) 2026 Scott Davis. See [LICENSE](LICENSE) for full text.
