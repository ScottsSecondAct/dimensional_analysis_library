# Project Structure

Copyright (c) 2026 Scott Davis — MIT License

---

## Directory Layout

```
dimensional_analysis_library/
│
├── include/                   Header-only library — copy these into your project
│   ├── dimensions.h           Core engine: Dimensions, DimAdd/Sub/Scale/Halve,
│   │                          IsQuantity, Quantity<Dim>, pow/sqrt/abs, operator<<
│   ├── units.h                User-facing header: type aliases, constants namespace,
│   │                          inline namespace si_literals with all UDLs
│   └── ecs.h                  Independent ECS sparse-set (no dependency on the above)
│
├── src/
│   └── main.cpp               Demo binary: exercises mechanics, chemistry,
│                              temperature, and ECS view iteration
│
├── tests/
│   └── unit_tests.cpp         GoogleTest suite — 117 tests, 16 suites
│
├── CMakeLists.txt             Builds engine_demo + engine_tests; fetches GoogleTest
│
├── README.md                  Project overview, quick-start, feature tables
├── MANUAL.md                  Full API reference and usage guide
├── ROADMAP.md                 Completed work, near-term additions, architectural limits
├── PROJECT_STRUCTURE.md       This file
├── LICENSE                    MIT License — Copyright (c) 2026 Scott Davis
└── CLAUDE.md                  Instructions for Claude Code (AI development assistant)
```

---

## Header Dependency Graph

```
Your code
  │
  │  #include "units.h"
  ▼
units.h
  │  defines: type aliases (Mass, Velocity, Force, ...)
  │           namespace constants (c, h, k_B, G, ...)
  │           inline namespace si_literals (all UDLs)
  │
  │  #include "dimensions.h"
  ▼
dimensions.h
     defines: Dimensions<M,L,T,I,K,N,J>
              DimAdd, DimSub, DimScale, DimHalve
              IsQuantity (C++20 concept)
              Quantity<Dim>
              pow<N>, sqrt, abs
              operator<<


ecs.h  ──────  no dependency on dimensions.h or units.h
     defines: TypeRegistry
              BasePool / ComponentPool<T>
              Registry
```

`ecs.h` is completely independent. It can be used with or without the dimensional analysis headers.

---

## Component Descriptions

### `include/dimensions.h` — Core Engine

The entire compile-time machinery lives here. It is intentionally kept free of any domain knowledge (no SI unit names, no physical constants) — that is `units.h`'s job.

**`Dimensions<M,L,T,I,K,N,J>`**

A pure type-tag. Seven `int` non-type template parameters encode the SI base unit exponents in this order:

| Slot | Base Unit | Symbol | Example |
|---|---|---|---|
| M | Mass | kg | Force: M=1 |
| L | Length | m | Force: L=1 |
| T | Time | s | Force: T=-2 |
| I | Electric current | A | Charge: I=1 |
| K | Thermodynamic temperature | K | Entropy: K=-1 |
| N | Amount of substance | mol | MolarMass: N=-1 |
| J | Luminous intensity | cd | Illuminance: J=1 |

All seven default to 0, so `Dimensions<1,2,-2>` is shorthand for `Dimensions<1,2,-2,0,0,0,0>`.

**`DimAdd<D1,D2>` / `DimSub<D1,D2>`**

Struct templates whose `::type` member is a `Dimensions<...>` with element-wise addition or subtraction. Used by `Quantity::operator*` and `Quantity::operator/` respectively. Multiplication of physical quantities corresponds to adding their dimension exponents; division corresponds to subtracting.

**`DimScale<D,N>`**

Multiplies all exponents by the integer `N`. Used by `pow<N>(q)`. Works for any integer including negative values (`pow<-1>` gives the inverse unit) and zero (`pow<0>` gives a dimensionless quantity with value 1).

**`DimHalve<D>`**

Divides all exponents by 2. Used by `sqrt(q)`. Contains seven `static_assert` statements — one per slot — verifying that each exponent is even. This makes odd-exponent `sqrt` a hard compile-time error with a clear message, rather than producing a silently wrong fractional-exponent type (which `int` cannot represent).

**`IsQuantity`**

A C++20 concept. A type satisfies it if and only if it has a `DimensionType` member and is exactly `Quantity<DimensionType>`. Constrains the `*` and `/` binary operators on `Quantity`, producing clean `"constraint not satisfied"` errors instead of multi-page substitution failures.

**`Quantity<Dim>`**

The single user-visible value type. Contains exactly one `double value` member. All operators are `constexpr` and `inline`. Size is guaranteed to equal `sizeof(double)` — the abstraction has no memory or runtime cost. `operator<=>` is defaulted, providing all six comparisons for same-dimension quantities.

**Free functions**

| Function | Signature | Description |
|---|---|---|
| `pow<N>` | `(Q q) -> Quantity<DimScale<Q::dim, N>>` | Integer power |
| `sqrt` | `(Q q) -> Quantity<DimHalve<Q::dim>>` | Square root; compile checks even exponents |
| `abs` | `(Q q) -> Q` | Absolute value; preserves type |
| `operator<<` | `(ostream&, Q) -> ostream&` | Prints `value [dim-string]` |

---

### `include/units.h` — User-Facing Header

Everything a user needs. Includes `dimensions.h` and adds domain-specific names and syntax.

**Type aliases**

`using Velocity = Quantity<Dimensions<0,1,-1>>;` and so on for all 42 named types. These are plain type aliases — no vtable, no wrapper, no overhead. They exist purely so that `static_assert(std::is_same_v<decltype(d/t), Velocity>)` reads like physics.

**`namespace constants`**

Twelve `inline constexpr` typed values. Using `inline constexpr` means each constant has a single definition across translation units (no ODR violation when included from multiple `.cpp` files). Three constants (`G`, `N_A`, `sigma`) use local `using` type aliases inside the namespace because their dimension patterns do not correspond to any globally-named type alias.

**`inline namespace si_literals`**

All UDL operators live here. `inline namespace` provides two guarantees simultaneously:
1. The UDLs are visible in the enclosing (global) namespace without any `using` declaration — backward compatible with code written before the namespace was added.
2. Users can write `using namespace si_literals` in headers or narrow scopes to make the dependency explicit.

Each UDL is defined twice — once taking `long double` (for floating-point literals like `1.5_kg`) and once taking `unsigned long long` (for integer literals like `2_kg`). The two overloads are identical in effect; the compiler selects based on whether the literal has a decimal point.

---

### `include/ecs.h` — Entity Component System

A self-contained sparse-set ECS in under 80 lines. No dependency on the dimensional analysis headers.

**`TypeRegistry`**

Uses a static template counter to assign a unique `int` ID to each component type at program start. `TypeRegistry::get_id<Position>()` always returns the same integer within a process, regardless of call order across translation units (C++ guarantees static-local initialization order within a translation unit; across TUs the IDs may differ between runs, but this is acceptable since the ECS lifetime is a single process).

**`ComponentPool<T>`**

Implements the sparse-set pattern:

```
Entity ID  →  sparse[entity_id]  →  index into dense[]
                                     dense[index] = component value
                                     entity_map[index] = entity_id (for iteration)
```

- `assign(entity, comp)` — O(1) amortised; resizes `sparse` if needed
- `get(entity)` — O(1); no bounds check on `dense` (entity must exist)
- `contains(entity)` — O(1); checks `sparse` bounds and sentinel `-1`
- `entities()` — returns the packed `entity_map` for cache-friendly iteration

**`Registry`**

Owns `ComponentPool` instances by type ID in an `unordered_map<int, unique_ptr<BasePool>>`. `BasePool` is a minimal abstract interface (`contains`, `size`, `virtual ~BasePool`) that allows the registry to store heterogeneous pools without knowing their component types.

`view<A, B, ...>(func)` iterates the first component type's pool (the "leader") and uses a C++17 fold expression `(pool.contains(entity) && ...)` to check all other components. It calls `func` only for entities present in every requested pool, with references to each component unpacked via the parameter pack.

---

### `src/main.cpp` — Demo Binary

Exercises three areas:

1. **Mechanics** — `10_m / 2_s` yields `Velocity`; `5_kg * 9.81_m/s²` yields `Force`
2. **Chemistry / Thermodynamics** — `1_kcal` stores 4184 J; `37_degC` stores 310.15 K; `1_atm` stores 101325 Pa
3. **ECS** — creates 5 entities, assigns `Position` to all and `VelocityComp` to even-indexed ones, then `view<Position, VelocityComp>` iterates only the 3 entities with both components

---

### `tests/unit_tests.cpp` — Test Suite

117 tests across 16 suites:

| Suite | Tests | What it covers |
|---|---|---|
| `DimEngine` | 2 | All 7 exponent slots propagate through `DimAdd` and `DimSub` |
| `Operators` | 8 | `+`, `-`, `*`, `/`, unary `-`, scalar `*`/`/` |
| `TypeCorrectness` | 9 | `static_assert` on result types of dimension arithmetic and new type aliases |
| `Comparisons` | 3 | `==`, `<`, `>` via `operator<=>` |
| `Conversions` | 32 | UDL values against known SI conversions (NIST/BIPM/IAU) |
| `Chemistry` | 2 | `Concentration * Volume = Amount`; `MolarMass` dimension |
| `Electro` | 2 | Ohm's law; `Current * Time = Charge` |
| `EMNamedUnits` | 6 | `_ohm`, `_F`, `_Wb`, `_T`, `_H`, `_S` type and value |
| `Constants` | 8 | Type and value of `c`, `h`, `k_B`, `e`, `m_e`, `G`, `N_A`, `R` |
| `Radiation` | 4 | `_Bq`, `_Gy`, `_Sv` types; `_Ci` conversion |
| `Photometry` | 3 | `_lm`, `_lx` types and `Illuminance` dimensions |
| `MathFunctions` | 7 | `pow<2>`, `pow<3>`, `pow<-1>`, `sqrt(Area)`, `sqrt(E/m)`, `abs` |
| `StreamOutput` | 3 | `operator<<` format; dimensionless shows `[1]` |
| `IntegerUDLs` | 5 | `unsigned long long` overloads match `long double` overloads |
| `ElectronicsUDLs` | 22 | Full prefix ladders for V, A, C, F, Ω, H; Ohm's law cross-check |
| `ECS` | 1 | `view<int, float>` filters correctly |

---

## Design Decisions

### Why `int` exponents rather than `std::ratio`?

`int` exponents cover all standard physics and engineering: every SI-derived unit has integer exponents. The design is simpler, faster to compile, and produces shorter type names in error messages. The cost is that `sqrt` of a quantity with odd exponents is a hard error rather than yielding a half-integer exponent type. This is a documented trade-off — see `ROADMAP.md` for the `std::ratio` migration path.

### Why a 7-slot pack rather than a map?

A fixed-size pack with defaulted parameters allows partial specialisation and is zero-overhead. A `std::map<int,int>` or similar runtime structure would be heap-allocated and non-`constexpr`. Every physics quantity ever needed in engineering fits within the 7 SI base dimensions.

### Why `Quantity<Dim>` rather than a derived type per unit?

A single template with a dimension type parameter means there are no virtual dispatch, no vtable, and no inheritance hierarchy. `Force` is just `Quantity<Dimensions<1,1,-2>>` — a type alias, not a class. The compiler treats all `Quantity` specialisations uniformly and optimises across them.

### Why `inline namespace si_literals`?

Plain `namespace si_literals` would require `using namespace si_literals` at every call site, breaking code that previously used the UDLs without a using declaration. A plain `namespace` also cannot be `using`-ed selectively in headers without polluting all includers. `inline namespace` provides both: global availability by default, and an explicit opt-in name for documentation and scoping.

### Why are `G`, `N_A`, and `sigma` not globally aliased?

Their dimension patterns do not correspond to any commonly named quantity. `G` has dimensions m³·kg⁻¹·s⁻², which has no standard engineering name. Exposing a global `GravType` alias would add noise to user code without benefit. They are local `using` declarations inside `namespace constants`, visible as `constants::_GravType` only to users who specifically need to store those values in typed variables.

### Why is `ecs.h` independent?

The ECS and the dimensional analysis library serve different purposes and have different inclusion cost. Users who want only dimensional analysis should not pay for ECS compilation, and vice versa. Keeping them independent also keeps each header's dependency surface minimal.
