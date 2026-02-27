# Manual

Copyright (c) 2026 Scott Davis — MIT License

A complete reference for the Dimensional Analysis Library. For project overview and motivation see `README.md`. For file layout and design decisions see `PROJECT_STRUCTURE.md`.

---

## Contents

1. [Getting Started](#1-getting-started)
2. [The Quantity Type](#2-the-quantity-type)
3. [Type Aliases](#3-type-aliases)
4. [Arithmetic Operators](#4-arithmetic-operators)
5. [Math Functions](#5-math-functions)
6. [Comparison Operators](#6-comparison-operators)
7. [Stream Output](#7-stream-output)
8. [User-Defined Literals](#8-user-defined-literals)
9. [Physical Constants](#9-physical-constants)
10. [Entity Component System](#10-entity-component-system)
11. [Common Patterns](#11-common-patterns)
12. [Pitfalls and Limitations](#12-pitfalls-and-limitations)

---

## 1. Getting Started

### Requirements

- Clang 18.1.3+ (required; see [Pitfalls](#12-pitfalls-and-limitations) for GCC notes)
- C++20

### Installation

The library is header-only. Copy `include/dimensions.h` and `include/units.h` into your project. For the ECS also copy `include/ecs.h`.

```bash
cp include/dimensions.h include/units.h /your/project/include/
```

### Your First Program

```cpp
#include "units.h"
#include <iostream>

int main() {
    // Typed quantities
    Mass     m = 70.0_kg;
    Velocity v = 10.0_m / 1.0_s;

    // Arithmetic produces correct types automatically
    Energy ke = 0.5 * m * v * v;

    // Print with dimension label
    std::cout << ke << "\n";   // "3500 [kg·m^2·s^-2]"
    std::cout << ke.value << " J\n";  // "3500 J"

    return 0;
}
```

Compile:

```bash
clang++-18 -std=c++20 -I include main.cpp -o main
```

---

## 2. The Quantity Type

### Definition

```cpp
template <typename Dim>
struct Quantity {
    double value;
    explicit constexpr Quantity(double v);
    // ... operators
};
```

`Quantity<Dim>` wraps exactly one `double`. Its size is always 8 bytes — identical to a raw `double`. All operations are `constexpr` and `inline`; the abstraction has zero runtime cost.

### Constructing a Quantity

The constructor is `explicit` to prevent accidental implicit conversions from raw numbers:

```cpp
Mass m = Mass(70.0);      // OK — explicit
Mass m = 70.0_kg;         // OK — via UDL (preferred)
Mass m = 70.0;            // compile error — implicit conversion blocked
```

### Accessing the Value

`.value` is the raw `double` in SI base units. Use it when calling functions that expect `double` (e.g. `std::sin`, `std::printf`) or when you need a dimensionless result:

```cpp
Velocity v = 25.0_m / 1.0_s;
std::cout << v.value;          // 25.0
std::printf("%.2f m/s\n", v.value);
double ratio = v.value / constants::c.value;  // β = v/c (dimensionless)
```

### The `DimensionType` Member

Each `Quantity` specialisation exposes its dimension as `::DimensionType`:

```cpp
static_assert(std::is_same_v<Velocity::DimensionType, Dimensions<0,1,-1>>);
```

---

## 3. Type Aliases

All aliases are defined in `units.h`. They are plain `using` declarations — no overhead, no inheritance.

### SI Base Types

| Alias | Dimensions | SI Unit |
|---|---|---|
| `Mass` | `<1,0,0>` | kg |
| `Length` | `<0,1,0>` | m |
| `Time` | `<0,0,1>` | s |
| `Current` | `<0,0,0,1>` | A |
| `Temperature` | `<0,0,0,0,1>` | K |
| `Amount` | `<0,0,0,0,0,1>` | mol |
| `Luminosity` | `<0,0,0,0,0,0,1>` | cd |

### Mechanics

| Alias | Dimensions | SI Unit |
|---|---|---|
| `Area` | `<0,2,0>` | m² |
| `Volume` | `<0,3,0>` | m³ |
| `Velocity` | `<0,1,-1>` | m/s |
| `Acceleration` | `<0,1,-2>` | m/s² |
| `Momentum` | `<1,1,-1>` | kg·m/s |
| `Force` | `<1,1,-2>` | N |
| `Energy` | `<1,2,-2>` | J |
| `Power` | `<1,2,-3>` | W |
| `Pressure` | `<1,-1,-2>` | Pa |
| `Frequency` | `<0,0,-1>` | Hz |
| `Density` | `<1,-3,0>` | kg/m³ |
| `DynamicViscosity` | `<1,-1,-1>` | Pa·s |
| `KinematicViscosity` | `<0,2,-1>` | m²/s |
| `Action` | `<1,2,-1>` | J·s |

### Electromagnetism

| Alias | Dimensions | SI Unit |
|---|---|---|
| `Charge` | `<0,0,1,1>` | C |
| `Voltage` | `<1,2,-3,-1>` | V |
| `Resistance` | `<1,2,-3,-2>` | Ω |
| `Capacitance` | `<-1,-2,4,2>` | F |
| `Inductance` | `<1,2,-2,-2>` | H |
| `MagneticFlux` | `<1,2,-2,-1>` | Wb |
| `MagneticField` | `<1,0,-2,-1>` | T |
| `Conductance` | `<-1,-2,3,2>` | S |
| `ElectricField` | `<1,1,-3,-1>` | V/m |

### Thermodynamics

| Alias | Dimensions | SI Unit |
|---|---|---|
| `Entropy` | `<1,2,-2,0,-1>` | J/K |
| `SpecificHeat` | `<0,2,-2,0,-1>` | J/(kg·K) |
| `ThermalConductivity` | `<1,1,-3,0,-1>` | W/(m·K) |

### Chemistry

| Alias | Dimensions | SI Unit |
|---|---|---|
| `MolarMass` | `<1,0,0,0,0,-1>` | kg/mol |
| `Concentration` | `<0,-3,0,0,0,1>` | mol/m³ |
| `CatalyticActivity` | `<0,0,-1,0,0,1>` | kat |
| `MolarEnergy` | `<1,2,-2,0,0,-1>` | J/mol |
| `MolarEntropy` | `<1,2,-2,0,-1,-1>` | J/(mol·K) |

### Radiation / Dosimetry

| Alias | Equivalent | SI Unit |
|---|---|---|
| `RadioactiveActivity` | `Frequency` | Bq (= 1/s) |
| `AbsorbedDose` | `<0,2,-2>` | Gy, Sv (= J/kg) |

Note: `RadioactiveActivity` and `Frequency` are the same type — both are `Quantity<Dimensions<0,0,-1>>`. The distinction is semantic.

### Photometry

| Alias | Equivalent | SI Unit |
|---|---|---|
| `LuminousFlux` | `Luminosity` | lm (= cd, sr is dimensionless) |
| `Illuminance` | `<0,-2,0,0,0,0,1>` | lx (= cd/m²) |

### Defining Your Own

For quantities not covered by the built-in aliases, define a local alias:

```cpp
#include "units.h"

// Specific angular momentum: m²/s (same as KinematicViscosity, different physics)
using SpecAngularMomentum = Quantity<Dimensions<0,2,-1>>;

// Jerk: m/s³
using Jerk = Quantity<Dimensions<0,1,-3>>;
```

---

## 4. Arithmetic Operators

### Quantity × Quantity → new dimension

Multiplying two quantities adds their exponents:

```cpp
Force   f = 10.0_N;
Length  d = 3.0_m;
Energy  e = f * d;    // {1,1,-2} + {0,1,0} = {1,2,-2} ✓

Mass    m = 5.0_kg;
Velocity v = 2.0_m / 1.0_s;
Momentum p = m * v;   // {1,0,0} + {0,1,-1} = {1,1,-1} ✓
```

### Quantity / Quantity → new dimension

Dividing subtracts exponents:

```cpp
Length   d = 100.0_m;
Time     t = 10.0_s;
Velocity v = d / t;   // {0,1,0} - {0,0,1} = {0,1,-1} ✓

Voltage v = 12.0_V;
Current i = 3.0_A;
Resistance r = v / i; // {1,2,-3,-1} - {0,0,0,1} = {1,2,-3,-2} ✓
```

### Quantity + Quantity / Quantity − Quantity

Addition and subtraction require identical dimension types. Mismatched dimensions are a compile error:

```cpp
Length a = 5.0_m;
Length b = 3.0_m;
Length c = a + b;     // OK: 8 m
Length d = a - b;     // OK: 2 m

Time t = 2.0_s;
auto bad = a + t;     // compile error: no matching operator+ for Length and Time
```

### Scalar × Quantity and Quantity × Scalar

Multiply or divide by a dimensionless `double` to scale a quantity:

```cpp
Length half = 10.0_m * 0.5;       // 5.0 m
Force  twice = 2.0 * 50.0_N;      // 100 N
Velocity reduced = 30.0_m/1.0_s / 3.0;  // 10 m/s
```

Note: scalar must be `double`, not `int` — use `2.0` not `2` for the scalar overload. Integer literals alone will not match the scalar overload (they satisfy `unsigned long long` UDL overloads instead on quantities with UDLs; on raw arithmetic expressions use explicit `double`).

### Unary Negation

```cpp
Force f  =  9.81_N;
Force mg = -9.81_N;   // -9.81 N, type is still Force
```

---

## 5. Math Functions

These are free functions defined in `dimensions.h`. They are found via argument-dependent lookup (ADL) — no `using` declaration required.

### `pow<N>(q)` — Integer Power

Raises a quantity to an integer power. All dimension exponents are scaled by `N`.

```cpp
Length side = 4.0_m;
Area   a    = pow<2>(side);   // 16 m²
Volume vol  = pow<3>(side);   // 64 m³

// Negative exponents give inverse units
Velocity   v   = 20.0_m / 1.0_s;
auto       inv = pow<-1>(v);  // s/m (useful for slowness, reciprocal speed)
```

`N` must be a compile-time integer constant (template argument):

```cpp
int n = 2;
pow<n>(side);   // compile error — n must be constexpr
pow<2>(side);   // OK
```

### `sqrt(q)` — Square Root

Returns a quantity whose dimension exponents are all halved. **Requires that all exponents are even** — this is checked at compile time.

```cpp
Area   a    = 9.0_m * 9.0_m;   // 81 m²
Length side = sqrt(a);          // 9 m ✓

// E = ½mv² → v = sqrt(2E/m)
Energy e = 800.0_J;
Mass   m = 4.0_kg;
Velocity v = sqrt(2.0 * e / m);  // sqrt({0,2,-2}) → Velocity ✓
EXPECT_DOUBLE_EQ(v.value, 20.0);

// Fails at compile time — time exponent is -1 (odd)
sqrt(Velocity{10.0});  // error: "sqrt: time exponent must be even"
```

### `abs(q)` — Absolute Value

Returns the absolute value of the quantity. The dimension type is unchanged.

```cpp
Length dist  = abs(-500.0_m);   // 500 m, type is Length
Force  mag   = abs(-9.81_N);    // 9.81 N, type is Force
```

---

## 6. Comparison Operators

`Quantity<Dim>` defaults `operator<=>`, which generates all six comparison operators. Comparisons are only valid between quantities of the **same dimension** — the compiler enforces this through the type system.

```cpp
Length a = 5.0_m;
Length b = 3.0_m;

a == b    // false
a != b    // true
a <  b    // false
a >  b    // true
a <= b    // false
a >= b    // true

// Works with different UDLs — both convert to SI first
Length m1 = 1.0_km;   // 1000 m
Length m2 = 900.0_m;
m1 > m2               // true
```

Comparing different dimensions is a compile error:

```cpp
Time t = 5.0_s;
Length d = 5.0_m;
d == t;   // compile error: no match for operator==
```

---

## 7. Stream Output

`operator<<` is defined for all `Quantity` specialisations. It prints the raw SI value followed by the dimension in square brackets.

```cpp
#include <iostream>
#include "units.h"

std::cout << 9.81_m / (1.0_s * 1.0_s);   // "9.81 [m·s^-2]"
std::cout << 101325.0_Pa;                  // "101325 [kg·m^-1·s^-2]"
std::cout << 1.0_eV;                       // "1.60218e-19 [kg·m^2·s^-2]"
std::cout << 300.0_K;                      // "300 [K]"
std::cout << 2.4_GHz;                      // "2.4e+09 [s^-1]"
```

Dimensionless quantities (all exponents zero) show `[1]`:

```cpp
using Dimensionless = Quantity<Dimensions<0,0,0,0,0,0,0>>;
std::cout << Dimensionless(0.707);   // "0.707 [1]"
```

The dimension string format:
- Slot names in order: `kg`, `m`, `s`, `A`, `K`, `mol`, `cd`
- Slots with exponent 0 are omitted
- Exponent 1 is omitted (e.g. `m` not `m^1`)
- Other exponents shown as `^N` (e.g. `m^2`, `s^-2`)
- Slots separated by `·` (UTF-8 middle dot, U+00B7)

---

## 8. User-Defined Literals

All UDLs are in `inline namespace si_literals`. They are available globally by default. You can also import them explicitly:

```cpp
using namespace si_literals;   // opt-in for documentation/clarity
```

Every UDL has two overloads:
- `long double` — for floating-point literals: `1.5_kg`, `9.81_m`
- `unsigned long long` — for integer literals: `70_kg`, `100_m`

Values are stored in SI base units. A UDL is a conversion function — `160.0_kn` stores `82.31 m/s`, not `160.0`.

### Mass

| UDL | Factor | Notes |
|---|---|---|
| `_kg` | × 1 | SI base |
| `_g` | × 10⁻³ | |
| `_mg` | × 10⁻⁶ | |
| `_Da` | × 1.660 539 × 10⁻²⁷ | Dalton — CODATA 2018 |
| `_u` | × 1.660 539 × 10⁻²⁷ | Unified atomic mass unit — same as `_Da` |
| `_tonne` | × 10³ | Metric tonne |
| `_lb` | × 0.453 592 37 | Pound-mass — exact (1959) |
| `_lbm` | × 0.453 592 37 | Alias for `_lb` |
| `_oz` | × 0.028 349 523 125 | Ounce = lb/16 — exact |
| `_slug` | × 14.593 902 937 | Imperial slug |

### Length

| UDL | Factor | Notes |
|---|---|---|
| `_m` | × 1 | SI base |
| `_km` | × 10³ | |
| `_cm` | × 10⁻² | |
| `_mm` | × 10⁻³ | |
| `_in` | × 0.0254 | Inch — exact (1959) |
| `_ft` | × 0.3048 | Foot — exact |
| `_yd` | × 0.9144 | Yard — exact |
| `_mi` | × 1609.344 | Statute mile — exact |
| `_nmi` | × 1852.0 | Nautical mile — exact |
| `_au` | × 1.495 978 707 × 10¹¹ | Astronomical unit — IAU 2012 exact |
| `_ly` | × 9.460 730 × 10¹⁵ | Light-year = c × Julian year |
| `_pc` | × 3.085 677 × 10¹⁶ | Parsec |
| `_kpc` | × 3.085 677 × 10¹⁹ | Kiloparsec |
| `_Mpc` | × 3.085 677 × 10²² | Megaparsec |

### Time

| UDL | Factor | Notes |
|---|---|---|
| `_s` | × 1 | SI base |
| `_ms` | × 10⁻³ | |
| `_us` | × 10⁻⁶ | Microsecond |
| `_min` | × 60 | |
| `_hr` | × 3600 | |
| `_day` | × 86 400 | |
| `_yr` | × 31 557 600 | Julian year = 365.25 days |

### Current, Temperature, Amount, Luminosity

| UDL | Factor | Notes |
|---|---|---|
| `_A` | × 1 | SI base |
| `_mA` | × 10⁻³ | |
| `_uA` | × 10⁻⁶ | |
| `_nA` | × 10⁻⁹ | |
| `_K` | × 1 | SI base (absolute) |
| `_degC` | + 273.15 | Converts Celsius to Kelvin |
| `_degF` | (v−32)×5/9 + 273.15 | Converts Fahrenheit to Kelvin |
| `_mol` | × 1 | SI base |
| `_mmol` | × 10⁻³ | |
| `_cd` | × 1 | SI base |

### Force

| UDL | Factor |
|---|---|
| `_N` | × 1 |
| `_kN` | × 10³ |
| `_lbf` | × 4.448 221 615 (exact: 1 lb × g_n) |

### Energy

| UDL | Factor | Notes |
|---|---|---|
| `_J` | × 1 | |
| `_kJ` | × 10³ | |
| `_cal` | × 4.184 | Thermochemical calorie |
| `_kcal` | × 4184.0 | |
| `_eV` | × 1.602 176 634 × 10⁻¹⁹ | Exact (2019 SI) |
| `_meV` | × 1.602 176 634 × 10⁻²² | |
| `_MeV` | × 1.602 176 634 × 10⁻¹³ | |
| `_GeV` | × 1.602 176 634 × 10⁻¹⁰ | |
| `_Wh` | × 3600 | Watt-hour |
| `_kWh` | × 3.6 × 10⁶ | |
| `_BTU` | × 1055.055 852 62 | International Table BTU |

### Power

| UDL | Factor |
|---|---|
| `_W` | × 1 |
| `_kW` | × 10³ |
| `_MW` | × 10⁶ |
| `_hp` | × 745.699 871 58 (mechanical horsepower) |

### Pressure

| UDL | Factor |
|---|---|
| `_Pa` | × 1 |
| `_kPa` | × 10³ |
| `_MPa` | × 10⁶ |
| `_bar` | × 10⁵ |
| `_atm` | × 101 325 |
| `_psi` | × 6894.757 293 168 |
| `_torr` | × 101 325 / 760 |
| `_mmHg` | × 133.322 387 415 |

### Frequency

| UDL | Factor |
|---|---|
| `_Hz` | × 1 |
| `_kHz` | × 10³ |
| `_MHz` | × 10⁶ |
| `_GHz` | × 10⁹ |

### Volume and Area

| UDL | Factor |
|---|---|
| `_L` | × 10⁻³ | Litre |
| `_mL` | × 10⁻⁶ | Millilitre |
| `_b` | × 10⁻²⁸ | Barn (nuclear cross-section) |

### Velocity

| UDL | Factor |
|---|---|
| `_kn` | × 1852 / 3600 | Knot — exact |

### Voltage

| UDL | Factor |
|---|---|
| `_MV` | × 10⁶ |
| `_kV` | × 10³ |
| `_V` | × 1 |
| `_mV` | × 10⁻³ |
| `_uV` | × 10⁻⁶ |

### Resistance

| UDL | Factor |
|---|---|
| `_Mohm` | × 10⁶ |
| `_kohm` | × 10³ |
| `_ohm` | × 1 |
| `_mohm` | × 10⁻³ |

### Capacitance

| UDL | Factor |
|---|---|
| `_F` | × 1 |
| `_mF` | × 10⁻³ |
| `_uF` | × 10⁻⁶ |
| `_nF` | × 10⁻⁹ |
| `_pF` | × 10⁻¹² |

### Inductance

| UDL | Factor |
|---|---|
| `_H` | × 1 |
| `_mH` | × 10⁻³ |
| `_uH` | × 10⁻⁶ |
| `_nH` | × 10⁻⁹ |

### Charge

| UDL | Factor |
|---|---|
| `_C` | × 1 |
| `_mC` | × 10⁻³ |
| `_uC` | × 10⁻⁶ |
| `_nC` | × 10⁻⁹ |
| `_pC` | × 10⁻¹² |

### Other EM

| UDL | Type | Factor |
|---|---|---|
| `_Wb` | `MagneticFlux` | × 1 |
| `_T` | `MagneticField` | × 1 |
| `_S` | `Conductance` | × 1 |

### Radiation / Dosimetry

| UDL | Type | Factor |
|---|---|---|
| `_Bq` | `RadioactiveActivity` | × 1 |
| `_Ci` | `RadioactiveActivity` | × 3.7 × 10¹⁰ |
| `_Gy` | `AbsorbedDose` | × 1 |
| `_Sv` | `AbsorbedDose` | × 1 |

### Photometry

| UDL | Type | Factor |
|---|---|---|
| `_lm` | `LuminousFlux` | × 1 |
| `_lx` | `Illuminance` | × 1 |

---

## 9. Physical Constants

All constants are in `namespace constants`, defined as `inline constexpr` typed values.

```cpp
#include "units.h"

// Access via qualified name
auto E = constants::m_e * pow<2>(constants::c);   // electron rest energy

// Or with a using declaration in a local scope
{
    using namespace constants;
    Energy E = m_e * pow<2>(c);
}
```

| Name | Value | Type | Source |
|---|---|---|---|
| `c` | 299 792 458 m/s | `Velocity` | 2019 SI (exact) |
| `h` | 6.626 070 15 × 10⁻³⁴ J·s | `Action` | 2019 SI (exact) |
| `hbar` | 1.054 571 817 × 10⁻³⁴ J·s | `Action` | 2019 SI (exact) |
| `e` | 1.602 176 634 × 10⁻¹⁹ C | `Charge` | 2019 SI (exact) |
| `k_B` | 1.380 649 × 10⁻²³ J/K | `Entropy` | 2019 SI (exact) |
| `R` | 8.314 462 618 J/(mol·K) | `MolarEntropy` | 2019 SI (exact) |
| `m_e` | 9.109 383 7015 × 10⁻³¹ kg | `Mass` | CODATA 2018 |
| `m_p` | 1.672 621 923 69 × 10⁻²⁷ kg | `Mass` | CODATA 2018 |
| `m_n` | 1.674 927 498 04 × 10⁻²⁷ kg | `Mass` | CODATA 2018 |
| `G` | 6.674 30 × 10⁻¹¹ m³/(kg·s²) | `constants::_GravType` | CODATA 2018 |
| `N_A` | 6.022 140 76 × 10²³ /mol | `constants::_InvAmtType` | 2019 SI (exact) |
| `sigma` | 5.670 374 419 × 10⁻⁸ W/(m²·K⁴) | `constants::_SBType` | Derived (exact) |

`_GravType`, `_InvAmtType`, and `_SBType` are `using` declarations local to `namespace constants`. To store these constants in a typed variable, use `auto` or `decltype`:

```cpp
auto G_copy = constants::G;         // type is constants::_GravType
decltype(constants::G) G2 = constants::G;
```

---

## 10. Entity Component System

`ecs.h` is independent of the dimensional analysis headers. Include it separately.

```cpp
#include "ecs.h"
```

### Concepts

- **Entity** — an integer ID. No entity class; any `int` is a valid entity.
- **Component** — any copyable/movable type. No base class required.
- **Pool** — stores all components of one type, indexed by entity.
- **Registry** — owns all pools; the main entry point.

### Creating a Registry

```cpp
Registry reg;
```

### Adding Components

```cpp
struct Position { float x, y; };
struct Health   { int current, max; };

reg.get_pool<Position>().assign(0, {1.0f, 2.0f});
reg.get_pool<Health>().assign(0, {100, 100});
reg.get_pool<Position>().assign(1, {5.0f, 0.0f});
// Entity 1 has no Health component
```

`assign(entity, component)` is O(1) amortised. Assigning to the same entity twice does **not** update the value — it appends a duplicate. Design your logic to assign once per entity.

### Accessing a Single Component

```cpp
Position& p = reg.get_pool<Position>().get(0);  // O(1)
p.x += 1.0f;

bool exists = reg.get_pool<Health>().contains(1);  // false — entity 1 has no Health
```

`get()` does not bounds-check. Calling it for an entity that was never assigned is undefined behaviour. Use `contains()` first if uncertain.

### Iterating with `view`

`view<Components...>(func)` iterates the first component type's pool and calls `func` for every entity that also has all remaining components.

```cpp
// Update position by velocity for all entities with both components
reg.view<Position, Velocity>([](Position& p, Velocity& v) {
    p.x += v.dx * dt;
    p.y += v.dy * dt;
});

// Read-only: just print
reg.view<Health>([](Health& h) {
    std::printf("hp: %d / %d\n", h.current, h.max);
});
```

The lambda receives references to each component in the same order as the template argument list. The first type in the list is the "leader" — the pool iterated. For best performance, list the rarest component first.

### Pool Size

```cpp
std::size_t n = reg.get_pool<Position>().size();   // number of entities with Position
```

---

## 11. Common Patterns

### Typed Function Signatures

Replace `double` parameters with typed quantities at API boundaries:

```cpp
// Before — which double is which?
double compute_force(double mass, double accel);

// After — mismatched arguments are a compile error
Force compute_force(Mass mass, Acceleration accel) {
    return mass * accel;
}
```

### Mixed-Unit Input

Accept inputs in natural units, store in SI internally:

```cpp
struct FlightState {
    Velocity  airspeed  = 250.0_kn;    // 128.6 m/s
    Length    altitude  = 35000_ft;    // 10668 m
    Mass      fuel      = 8200_kg;
    Pressure  qc        = 0.0_Pa;      // computed below
};
```

### Physics Formulas

Write formulas in the same form as the textbook — the type system verifies dimensional homogeneity:

```cpp
// Kinetic energy: KE = ½mv²
Energy ke(Mass m, Velocity v) { return 0.5 * m * v * v; }

// Escape velocity: v = sqrt(2GM/r)
Velocity escape(Mass planet, Length radius) {
    return sqrt(2.0 * constants::G.value * planet.value / radius.value);
    //         ↑ G has no global alias — use .value for intermediate double arithmetic
}

// Ohm's law: P = V²/R
Power dissipation(Voltage v, Resistance r) { return v * v / r; }

// RC time constant
Time rc_tau(Resistance r, Capacitance c) { return r * c; }
```

### Using `auto` for Intermediate Results

When the intermediate type is not important by name, `auto` reads cleanly:

```cpp
auto force   = 10.0_kg * 9.81_m / (1.0_s * 1.0_s);   // Force
auto voltage = force / (1.5_A * 2.0_s / 1.0_m);       // some EM quantity
auto area    = pow<2>(5.0_m);                          // Area
```

### Extracting Raw Values

`.value` is always in SI base units:

```cpp
Velocity v = 100.0_kn;          // stored as 51.44 m/s
double mph = v.value * 2.23694; // convert to mph manually
std::sin(v.value);               // trig function takes double
```

---

## 12. Pitfalls and Limitations

### `sqrt` Requires Even Exponents

The dimension exponents must all be even, or the static assert fires:

```cpp
sqrt(16.0_m * 16.0_m);   // Area{0,2,0} → Length ✓
sqrt(8.0_J / 2.0_kg);    // {0,2,-2} → Velocity ✓

sqrt(10.0_m / 1.0_s);    // {0,1,-1}: length=1 odd → compile error
```

For quantities with odd exponents, compute via `.value` and re-wrap:

```cpp
Velocity v = 25.0_m / 1.0_s;
// Can't sqrt(v), but can do:
double speed = v.value;
double half_power = std::sqrt(speed);   // raw double, no type safety
```

### Temperature Addition Is Dimensionally Valid But Physically Meaningless

The library tracks dimensions, not affine offsets. `0_degC` stores `273.15 K`. Adding two temperatures is always treated as adding two absolute temperatures:

```cpp
Temperature boiling  = 100_degC;  // 373.15 K
Temperature freezing =   0_degC;  // 273.15 K

// Dimensionally valid, physically nonsense:
Temperature sum = boiling + freezing;   // 646.3 K — not a meaningful quantity
```

For temperature *differences*, this works correctly because the offset cancels:

```cpp
// ΔT is fine if you compute it as a difference of two same-scale temperatures
// and only use the result as a ΔT, not an absolute temperature.
auto delta = boiling - freezing;   // 100 K — correct delta
```

### No `std::sin` / `std::cos` on `Quantity`

Transcendental functions take `double`. Extract `.value` explicitly:

```cpp
double angle_rad = 45.0 * M_PI / 180.0;
Length x = distance * std::cos(angle_rad);   // won't compile — need:
Length x = distance * std::cos(angle_rad);   // distance.value not needed here
                                              // because scalar * Quantity is defined
// But:
double coeff = std::sin(angle_rad);           // double — then multiply
Length x = dist * coeff;                      // OK
```

### Scalar Must Be `double`

The scalar multiply and divide operators take `double`. Passing an untyped integer literal can be ambiguous:

```cpp
Length d = 10.0_m * 2;     // may be ambiguous — prefer 2.0
Length d = 10.0_m * 2.0;   // unambiguous scalar multiply
```

### Duplicate ECS Assignments Are Not Updates

`ComponentPool::assign` always appends:

```cpp
reg.get_pool<int>().assign(0, 10);
reg.get_pool<int>().assign(0, 20);   // entity 0 now has TWO entries
                                     // get(0) still returns 10
```

Assign once per entity per component type. If you need update semantics, use `get()` after the initial `assign`:

```cpp
reg.get_pool<int>().assign(0, 10);
reg.get_pool<int>().get(0) = 20;    // update ✓
```

### Compiler Requirement

The library requires Clang 18.1.3+. GCC support has not been tested. The primary dependency is correct C++20 concept instantiation and `constexpr` evaluation behaviour.
