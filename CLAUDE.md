# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Build & Run

Requires Clang 18.1.3+ and CMake 3.20+. GoogleTest is fetched automatically via `FetchContent`.

```bash
# Set compiler (required)
export CXX=clang++-18
export CC=clang-18

# Configure (first time or after CMakeLists changes)
cmake -G Ninja -B build -S .

# Build everything
cmake --build build -j$(nproc)

# Run tests
./build/engine_tests

# Run demo
./build/engine_demo
```

There is no install step — this is a header-only library with a demo and test binary.

## Architecture

The project has two independent header-only components in `include/`:

### `dimensions.h` — Compile-time dimensional analysis (core engine)
- `Dimensions<M,L,T,I,K,N,J>` is a 7-slot integer template pack encoding SI base unit exponents (Mass, Length, Time, Current, Temp, Amount, Luminosity).
- `DimAdd<D1,D2>` / `DimSub<D1,D2>` perform compile-time exponent addition/subtraction across all 7 slots.
- `IsQuantity` C++20 concept constrains cross-quantity operators.
- `Quantity<Dim>` wraps a `double` with dimension type info. All math is `constexpr`/`inline` for zero runtime overhead. A `Quantity` is exactly 8 bytes — same as `double`. Operators: `*`, `/`, `+`, `-`, unary `-`, scalar `*`, scalar `/`, `<=>`.

### `units.h` — User-facing header (include this)
- Includes `dimensions.h` and defines all SI type aliases and UDLs.
- **SI base aliases**: `Mass`, `Length`, `Time`, `Current`, `Temperature`, `Amount`, `Luminosity`.
- **Mechanics**: `Area`, `Volume`, `Velocity`, `Acceleration`, `Momentum`, `Force`, `Energy`, `Power`, `Pressure`, `Frequency`.
- **Electromagnetism**: `Charge`, `Voltage`, `Resistance`, `Capacitance`, `Inductance`, `MagneticFlux`, `MagneticField`.
- **Thermodynamics**: `Entropy`, `SpecificHeat`, `ThermalConductivity`.
- **Chemistry**: `MolarMass`, `Concentration`, `CatalyticActivity`, `MolarEnergy`, `MolarEntropy`.
- **UDLs**: prefixed base units (`_kg`, `_g`, `_m`, `_km`, `_s`, `_ms`, `_A`, `_K`, `_mol`, `_cd`, …) plus derived/non-SI (`_N`, `_J`, `_kcal`, `_eV`, `_W`, `_Pa`, `_atm`, `_Hz`, `_L`, `_V`, …). Temperature offsets `_degC` and `_degF` convert to Kelvin on construction.

### `ecs.h` — Entity Component System (sparse set)
- `TypeRegistry` assigns unique integer IDs to component types at runtime using a static counter template.
- `ComponentPool<T>` implements a sparse-set: a `sparse` index vector maps entity IDs to positions in the packed `dense` vector, enabling O(1) add/lookup and cache-friendly iteration.
- `Registry` owns pools by type ID and exposes `view<Components...>(func)` which iterates the smallest pool (first type in pack) and calls `func` only for entities that have all requested components.

### `src/main.cpp` — Demo
Exercises both systems: dimensional arithmetic using UDLs, then ECS entity creation and component view iteration.

### `tests/unit_tests.cpp` — GoogleTest suite
Tests: `DimEngine` (all 7 slots via DimAdd/DimSub), `Operators` (`+`, `-`, `*`, `/`, unary, scalar), `TypeCorrectness` (static_assert on result types), `Comparisons` (spaceship), `Conversions` (UDL SI values), `Chemistry`, `Electro`, `ECS`.
