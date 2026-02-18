# Zero-Overhead Dimensional Analysis Library (C++20/23)

**Problem Statement:** Create a High-Performance Type-Safe Physics Library

- **High-Level Goal:** Implement a library where physical units (Mass, Length, Time) are enforced at compile-time. Operations like `Length / Time` must automatically result in a `Velocity` type with zero runtime overhead.
- **Core Concepts:** Template Metaprogramming, Variadic Templates, C++20 Concepts, User-Defined Literals (UDLs).
- **Architectural Challenges:** Eliminating abstraction penalties so that `Quantity` math is as fast as raw `double` math. Ensuring human-readable compiler errors for unit mismatches.
- **Tooling:** Clang 18.1.3, CMake (Ninja), GoogleTest.

## Solution
A modern, header-only C++20 library designed to demonstrate zero-overhead abstraction and compile-time dimensional safety. 

By leveraging variadic integer packs to represent SI base unit exponents, the library performs physics calculations during the compilation phase, turning unit errors into compiler failures rather than runtime bugs.

### Architectural Highlights

* **Variadic Dimension Engine:** Uses a 7-slot integer template pack to represent the SI base units (Mass, Length, Time, Current, Temp, Amount, Luminosity).
* **Zero-Cost Abstraction:** Every operation is `constexpr` and `inline`. A `Quantity<Length>` occupies the exact same memory footprint (8 bytes) as a standard `double`.
* **C++20 Concepts Enforcement:** Employs the `IsQuantity` concept to restrict mathematical operators, ensuring that only valid physical dimensions can be combined.
* **User-Defined Literals:** Supports native-feeling syntax such as `auto dist = 10.5_m;` and `auto duration = 2.0_s;`.



### Prerequisites

To build and run this project, you must have a compiler that fully supports C++20/23 standards.
* **Compiler:** Clang 18.1.3 or newer (Strictly required for full feature support).
* **Build System:** CMake 3.20 or newer.
* **Debugger:** GDB (configured with pretty-printing for template inspection).

### Build Instructions

This project uses CMake and Ninja to orchestrate the build and automatically fetches GoogleTest for unit testing.

```bash
# 1. Ensure Clang 18 is the active compiler
export CXX=clang++-18
export CC=clang-18

# 2. Configure the build directory
cmake -G Ninja -B build -S .

# 3. Compile the demonstration and test binaries
cmake --build build -j$(nproc)

# 4. Run the unit tests
./build/engine_tests

# 5. Run the demonstration
./build/engine_demo
```

## Acknowledgments
The problem statement, architectural blueprints, C++20/23 metaprogramming patterns, and build system configurations for this project were developed in collaboration with Gemini as part of an advanced software engineering exercise.