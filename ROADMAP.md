# Roadmap

Copyright (c) 2026 Scott Davis — MIT License

This document tracks completed work, near-term additions, and longer-term architectural directions for the Dimensional Analysis Library.

---

## Completed

### Core Engine (`dimensions.h`)

- [x] **`Dimensions<M,L,T,I,K,N,J>`** — 7-slot integer template pack encoding all SI base unit exponents (mass, length, time, current, temperature, amount, luminosity)
- [x] **`DimAdd<D1,D2>` / `DimSub<D1,D2>`** — compile-time exponent addition and subtraction for `*` and `/` operators
- [x] **`DimScale<D,N>`** — multiply all exponents by integer N, used by `pow<N>`
- [x] **`DimHalve<D>`** — divide all exponents by 2, used by `sqrt`; static asserts enforce that all exponents are even (otherwise result requires fractional representation)
- [x] **`IsQuantity` concept** — C++20 concept constraining `*` and `/` to valid `Quantity` types; produces clear diagnostics instead of substitution failures
- [x] **`Quantity<Dim>`** — zero-overhead wrapper around `double`; `sizeof == 8`; all operators `constexpr` and `inline`
- [x] **Operators**: `*`, `/`, `+`, `-`, unary `-`, scalar `*`, scalar `/`, `<=>`
- [x] **`operator<=>`** defaulted — enables all six comparisons (`==`, `!=`, `<`, `>`, `<=`, `>=`) on same-dimension quantities
- [x] **`pow<N>(q)`** — integer power; scales all dimension exponents by N; works for positive, negative, and zero N
- [x] **`sqrt(q)`** — square root with compile-time even-exponent check
- [x] **`abs(q)`** — absolute value; preserves dimension type
- [x] **`operator<<`** — stream output in the form `9.81 [m·s^-2]`; dimensionless quantities show `[1]`

### Type Aliases (`units.h`)

- [x] **7 SI base types**: `Mass`, `Length`, `Time`, `Current`, `Temperature`, `Amount`, `Luminosity`
- [x] **Mechanics** (14 types): `Area`, `Volume`, `Velocity`, `Acceleration`, `Momentum`, `Force`, `Energy`, `Power`, `Pressure`, `Frequency`, `Density`, `DynamicViscosity`, `KinematicViscosity`, `Action`
- [x] **Electromagnetism** (9 types): `Charge`, `Voltage`, `Resistance`, `Capacitance`, `Inductance`, `MagneticFlux`, `MagneticField`, `Conductance`, `ElectricField`
- [x] **Thermodynamics** (3 types): `Entropy`, `SpecificHeat`, `ThermalConductivity`
- [x] **Chemistry** (5 types): `MolarMass`, `Concentration`, `CatalyticActivity`, `MolarEnergy`, `MolarEntropy`
- [x] **Radiation / Dosimetry** (2 types): `RadioactiveActivity`, `AbsorbedDose`
- [x] **Photometry** (2 types): `LuminousFlux`, `Illuminance`

### Physical Constants (`namespace constants`)

- [x] **Exact (2019 SI redefinition)**: `c`, `h`, `hbar`, `e`, `k_B`, `R`, `N_A`
- [x] **CODATA 2018 measured**: `m_e`, `m_p`, `m_n`, `G`, `sigma`
- [x] All constants carry their correct dimension type — mixing them incorrectly is a compile error

### User-Defined Literals (`inline namespace si_literals`)

- [x] **Dual overloads** — every UDL accepts both `long double` (e.g. `1.5_kg`) and `unsigned long long` (e.g. `2_kg`)
- [x] **140+ UDLs** across mass, length, time, energy, EM, pressure, power, force, area, velocity, radiation, photometry
- [x] **`inline namespace si_literals`** — UDLs available globally by default; also importable explicitly with `using namespace si_literals`
- [x] **Temperature affine conversion** — `_degC` and `_degF` convert to Kelvin at construction

### Entity Component System (`ecs.h`)

- [x] **`TypeRegistry`** — assigns unique integer IDs to component types at compile time via static counter template
- [x] **`ComponentPool<T>`** — sparse-set with O(1) add, O(1) lookup, cache-friendly packed `dense` array
- [x] **`Registry`** — owns pools by type ID; `view<Components...>(func)` iterates smallest pool, filters by all requested components

### Testing & Build

- [x] **117 tests, 16 suites** — covers dimension arithmetic, type propagation, UDL conversion accuracy, physical constants, composite physics formulas, ECS filtering
- [x] **GoogleTest via `FetchContent`** — no manual test dependency setup
- [x] **CMake + Ninja build system**

---

## Near-Term (No Architectural Changes Required)

These additions fit within the current `int`-exponent design and do not require breaking changes.

### Standard Algorithm Compatibility

- [ ] **`std::numeric_limits<Quantity<D>>` specialization** — allows `Quantity` to be used with standard algorithms that query numeric properties (e.g. `std::clamp`, range checks). Delegates to `std::numeric_limits<double>`.

### Utility Functions

- [ ] **`clamp(q, lo, hi)`** — clamps a `Quantity` to `[lo, hi]`; all three must share the same dimension (enforced by type)
- [ ] **`min(a, b)` / `max(a, b)`** — dimension-preserving wrappers around `std::min`/`std::max`; `operator<=>` already makes these correct, but explicit wrappers improve discoverability and ADL

### Additional UDL Prefixes

- [ ] **`_nm`, `_um`, `_pm`** — nanometre, micrometre, picometre (optics, semiconductor geometry)
- [ ] **`_ns`, `_ps`, `_fs`** — nanosecond, picosecond, femtosecond (signal timing, laser physics)
- [ ] **`_mK`, `_uK`, `_nK`** — milli/micro/nano Kelvin (ultracold physics)
- [ ] **`_THz`** — terahertz (photonics, spectroscopy)
- [ ] **`_kJ/mol`** composite** — MolarEnergy convenience literal

### Demo Expansion

- [ ] **Extend `src/main.cpp`** to exercise physical constants, `pow<N>`, `sqrt`, `operator<<`, and `using namespace si_literals` explicitly

---

## Medium-Term (Design Work Required)

These additions are feasible within the current framework but require new design decisions.

### Compile-Time Unit String

Currently `operator<<` builds the dimension string at runtime using a small loop over dimension integers. A fully `constexpr` unit string (usable in `static_assert` messages or template diagnostics) would require:

- A `constexpr` string concatenation approach (fixed-size char array or `std::array<char,N>`)
- Computing the required buffer size at compile time

This would enable error messages like:
```
error: cannot add [kg·m²·s⁻²] and [kg·m·s⁻¹]
```

### Named Unit Formatting

The current `operator<<` always uses SI base unit notation (`kg·m^2·s^-2`). A named-unit formatter would recognize common combinations and print `J` instead of `kg·m^2·s^-2`, `N` instead of `kg·m·s^-2`, etc.

This requires a runtime lookup table mapping known dimension fingerprints to symbol strings — straightforward but needs a decision on what to do when a dimension matches no named unit.

### `cbrt` — Cube Root

Cube root requires that all exponents are divisible by 3. A `DimTrisect<D>` struct analogous to `DimHalve<D>` would implement this with static asserts. Less commonly needed than `sqrt` but useful for volume-to-length conversions.

---

## Long-Term (Architectural Redesign)

These require replacing the `int` exponent pack with a more expressive representation. They are **breaking changes** — existing user code would need to be recompiled.

### Fractional Exponents via `std::ratio`

The fundamental limitation of the current design is that dimension exponents are `int`. This means `sqrt(Velocity)` is a compile error (time exponent is `-1`, which cannot be halved exactly). Many real formulas involve fractional exponents:

- Dispersion: `v ∝ √(T/μ)` (wave speed in a string)
- Boundary layer: `δ ∝ √(νx/U)` (fluid mechanics)
- Stefan radiation: `T ∝ (P/σ)^(1/4)`

Replacing `int` with `std::ratio`:

```cpp
// Current
template <int M, int L, int T, ...>
struct Dimensions { ... };

// With fractional exponents
template <typename M, typename L, typename T, ...>  // M, L, T are std::ratio<N,D>
struct Dimensions { ... };
```

This would make `DimHalve` unnecessary (replaced by general rational arithmetic), enable `sqrt` on any dimension, and support `pow<1,3>` for cube roots. The cost is significantly more complex template machinery and longer compile times.

### Strongly-Typed `Angle`

Angles (radians, degrees) are dimensionless in SI — they have no mass, length, time, or any other base dimension. This means `Angle` would be `Quantity<Dimensions<0,0,...>>`, which is the same type as any other dimensionless result (e.g., a coefficient of friction). The type system cannot distinguish them.

A strongly-typed `Angle` requires a separate tag type outside the SI dimension model:

```cpp
struct AngleTag {};
template <typename Tag = void>
struct StrongQuantity { double value; };
using Angle = StrongQuantity<AngleTag>;
```

This would be a separate, parallel type system alongside `Quantity<Dim>` — feasible but adds significant API surface area.

### Offset Temperature Scales

The library currently tracks dimensions, not affine scales. `0_degC` converts to `273.15 K` at construction and is thereafter treated as an absolute temperature. This means `37_degC - 36_degC` gives `584.3 K`, not `1 K` — dimensionally valid but physically surprising.

Proper offset temperature support requires an affine quantity type that carries an origin offset, separate from the linear `Quantity<Dim>`. This is the same problem as the `Angle` type: it lives outside the pure dimensional analysis model.

---

## Known Limitations (By Design)

These are not planned for resolution — they are documented trade-offs of the `int`-exponent design.

| Limitation | Root Cause | Workaround |
|---|---|---|
| `sqrt(Velocity)` is a compile error | Time exponent `-1` is odd | Compute via `.value`, then wrap manually |
| `Angle` is untyped | Dimensionless in SI; same type as any ratio | Use raw `double` with a comment |
| `37_degC - 36_degC ≠ 1 K` intuitively | No affine scale tracking | Document and test carefully |
| No `std::sin`, `std::cos` on `Quantity` | Transcendental functions expect `double` | Use `q.value` for trig inputs |
| No `std::format` support | No `std::formatter<Quantity<D>>` specialization | Use `operator<<` or `.value` |
