#pragma once
#include "dimensions.h"

// =============================================================================
// SI Base Units
// =============================================================================

using Mass        = Quantity<Dimensions<1,0,0>>;
using Length      = Quantity<Dimensions<0,1,0>>;
using Time        = Quantity<Dimensions<0,0,1>>;
using Current     = Quantity<Dimensions<0,0,0,1>>;
using Temperature = Quantity<Dimensions<0,0,0,0,1>>;
using Amount      = Quantity<Dimensions<0,0,0,0,0,1>>;
using Luminosity  = Quantity<Dimensions<0,0,0,0,0,0,1>>;

// =============================================================================
// Mechanics Derived Units
// =============================================================================

using Area               = Quantity<Dimensions<0,2,0>>;
using Volume             = Quantity<Dimensions<0,3,0>>;
using Velocity           = Quantity<Dimensions<0,1,-1>>;
using Acceleration       = Quantity<Dimensions<0,1,-2>>;
using Momentum           = Quantity<Dimensions<1,1,-1>>;
using Force              = Quantity<Dimensions<1,1,-2>>;
using Energy             = Quantity<Dimensions<1,2,-2>>;
using Power              = Quantity<Dimensions<1,2,-3>>;
using Pressure           = Quantity<Dimensions<1,-1,-2>>;
using Frequency          = Quantity<Dimensions<0,0,-1>>;
using Density            = Quantity<Dimensions<1,-3,0>>;
using DynamicViscosity   = Quantity<Dimensions<1,-1,-1>>;
using KinematicViscosity = Quantity<Dimensions<0,2,-1>>;
using Action             = Quantity<Dimensions<1,2,-1>>;   // J·s; angular momentum, Planck h

// =============================================================================
// Electromagnetism Derived Units
// =============================================================================

using Charge        = Quantity<Dimensions<0,0,1,1>>;
using Voltage       = Quantity<Dimensions<1,2,-3,-1>>;
using Resistance    = Quantity<Dimensions<1,2,-3,-2>>;
using Capacitance   = Quantity<Dimensions<-1,-2,4,2>>;
using Inductance    = Quantity<Dimensions<1,2,-2,-2>>;
using MagneticFlux  = Quantity<Dimensions<1,2,-2,-1>>;
using MagneticField = Quantity<Dimensions<1,0,-2,-1>>;
using Conductance   = Quantity<Dimensions<-1,-2,3,2>>;   // S = 1/Ω
using ElectricField = Quantity<Dimensions<1,1,-3,-1>>;   // V/m

// =============================================================================
// Thermodynamics Derived Units
// =============================================================================

using Entropy             = Quantity<Dimensions<1,2,-2,0,-1>>;
using SpecificHeat        = Quantity<Dimensions<0,2,-2,0,-1>>;
using ThermalConductivity = Quantity<Dimensions<1,1,-3,0,-1>>;

// =============================================================================
// Chemistry Derived Units
// =============================================================================

using MolarMass         = Quantity<Dimensions<1,0,0,0,0,-1>>;
using Concentration     = Quantity<Dimensions<0,-3,0,0,0,1>>;
using CatalyticActivity = Quantity<Dimensions<0,0,-1,0,0,1>>;
using MolarEnergy       = Quantity<Dimensions<1,2,-2,0,0,-1>>;
using MolarEntropy      = Quantity<Dimensions<1,2,-2,0,-1,-1>>;

// =============================================================================
// Radiation / Dosimetry
// =============================================================================

using RadioactiveActivity = Frequency;                    // Bq = 1/s (dimensionally Hz)
using AbsorbedDose        = Quantity<Dimensions<0,2,-2>>; // J/kg — Gray (Gy), Sievert (Sv)

// =============================================================================
// Photometry
// =============================================================================

using LuminousFlux = Luminosity;                                  // lm = cd·sr (sr dimensionless)
using Illuminance  = Quantity<Dimensions<0,-2,0,0,0,0,1>>;        // lx = cd/m²

// =============================================================================
// Physical Constants (2019 SI redefinition — exact values; CODATA 2018 measured)
// =============================================================================

namespace constants {
    // Exact (2019 SI redefinition)
    inline constexpr Velocity     c   {299'792'458.0};
    inline constexpr Action       h   {6.62607015e-34};
    inline constexpr Action       hbar{1.054571817e-34};
    inline constexpr Charge       e   {1.602176634e-19};
    inline constexpr Entropy      k_B {1.380649e-23};
    inline constexpr MolarEntropy R   {8.314462618};

    // CODATA 2018 measured
    inline constexpr Mass         m_e {9.1093837015e-31};
    inline constexpr Mass         m_p {1.67262192369e-27};
    inline constexpr Mass         m_n {1.67492749804e-27};

    // Types needed only for these constants
    using _GravType   = Quantity<Dimensions<-1,3,-2>>;      // m³/(kg·s²)
    using _InvAmtType = Quantity<Dimensions<0,0,0,0,0,-1>>; // 1/mol
    using _SBType     = Quantity<Dimensions<1,0,-3,0,-4>>;  // W/(m²·K⁴)

    inline constexpr _GravType   G    {6.67430e-11};
    inline constexpr _InvAmtType N_A  {6.02214076e23};
    inline constexpr _SBType     sigma{5.670374419e-8};
}

// =============================================================================
// UDLs — all inside inline namespace so users can opt in explicitly:
//   using namespace si_literals;
// or rely on the inline namespace making them available globally by default.
// =============================================================================

inline namespace si_literals {

// --- Mass (SI base: kg) ---
constexpr Mass operator""_kg    (long double v)      { return Mass(static_cast<double>(v)); }
constexpr Mass operator""_kg    (unsigned long long v){ return Mass(static_cast<double>(v)); }
constexpr Mass operator""_g     (long double v)      { return Mass(static_cast<double>(v) * 1e-3); }
constexpr Mass operator""_g     (unsigned long long v){ return Mass(static_cast<double>(v) * 1e-3); }
constexpr Mass operator""_mg    (long double v)      { return Mass(static_cast<double>(v) * 1e-6); }
constexpr Mass operator""_mg    (unsigned long long v){ return Mass(static_cast<double>(v) * 1e-6); }
constexpr Mass operator""_Da    (long double v)      { return Mass(static_cast<double>(v) * 1.66053906660e-27); }
constexpr Mass operator""_Da    (unsigned long long v){ return Mass(static_cast<double>(v) * 1.66053906660e-27); }
constexpr Mass operator""_u     (long double v)      { return Mass(static_cast<double>(v) * 1.66053906660e-27); }
constexpr Mass operator""_u     (unsigned long long v){ return Mass(static_cast<double>(v) * 1.66053906660e-27); }
constexpr Mass operator""_tonne (long double v)      { return Mass(static_cast<double>(v) * 1e3); }
constexpr Mass operator""_tonne (unsigned long long v){ return Mass(static_cast<double>(v) * 1e3); }
constexpr Mass operator""_lb    (long double v)      { return Mass(static_cast<double>(v) * 0.45359237); }
constexpr Mass operator""_lb    (unsigned long long v){ return Mass(static_cast<double>(v) * 0.45359237); }
constexpr Mass operator""_lbm   (long double v)      { return Mass(static_cast<double>(v) * 0.45359237); }
constexpr Mass operator""_lbm   (unsigned long long v){ return Mass(static_cast<double>(v) * 0.45359237); }
constexpr Mass operator""_oz    (long double v)      { return Mass(static_cast<double>(v) * 0.028349523125); }
constexpr Mass operator""_oz    (unsigned long long v){ return Mass(static_cast<double>(v) * 0.028349523125); }
constexpr Mass operator""_slug  (long double v)      { return Mass(static_cast<double>(v) * 14.593902937); }
constexpr Mass operator""_slug  (unsigned long long v){ return Mass(static_cast<double>(v) * 14.593902937); }

// --- Length (SI base: m) ---
constexpr Length operator""_m   (long double v)      { return Length(static_cast<double>(v)); }
constexpr Length operator""_m   (unsigned long long v){ return Length(static_cast<double>(v)); }
constexpr Length operator""_km  (long double v)      { return Length(static_cast<double>(v) * 1e3); }
constexpr Length operator""_km  (unsigned long long v){ return Length(static_cast<double>(v) * 1e3); }
constexpr Length operator""_cm  (long double v)      { return Length(static_cast<double>(v) * 1e-2); }
constexpr Length operator""_cm  (unsigned long long v){ return Length(static_cast<double>(v) * 1e-2); }
constexpr Length operator""_mm  (long double v)      { return Length(static_cast<double>(v) * 1e-3); }
constexpr Length operator""_mm  (unsigned long long v){ return Length(static_cast<double>(v) * 1e-3); }
constexpr Length operator""_in  (long double v)      { return Length(static_cast<double>(v) * 0.0254); }
constexpr Length operator""_in  (unsigned long long v){ return Length(static_cast<double>(v) * 0.0254); }
constexpr Length operator""_ft  (long double v)      { return Length(static_cast<double>(v) * 0.3048); }
constexpr Length operator""_ft  (unsigned long long v){ return Length(static_cast<double>(v) * 0.3048); }
constexpr Length operator""_yd  (long double v)      { return Length(static_cast<double>(v) * 0.9144); }
constexpr Length operator""_yd  (unsigned long long v){ return Length(static_cast<double>(v) * 0.9144); }
constexpr Length operator""_mi  (long double v)      { return Length(static_cast<double>(v) * 1609.344); }
constexpr Length operator""_mi  (unsigned long long v){ return Length(static_cast<double>(v) * 1609.344); }
constexpr Length operator""_nmi (long double v)      { return Length(static_cast<double>(v) * 1852.0); }
constexpr Length operator""_nmi (unsigned long long v){ return Length(static_cast<double>(v) * 1852.0); }
constexpr Length operator""_au  (long double v)      { return Length(static_cast<double>(v) * 1.495978707e11); }
constexpr Length operator""_au  (unsigned long long v){ return Length(static_cast<double>(v) * 1.495978707e11); }
constexpr Length operator""_ly  (long double v)      { return Length(static_cast<double>(v) * 9.4607304725808e15); }
constexpr Length operator""_ly  (unsigned long long v){ return Length(static_cast<double>(v) * 9.4607304725808e15); }
constexpr Length operator""_pc  (long double v)      { return Length(static_cast<double>(v) * 3.085677581491367e16); }
constexpr Length operator""_pc  (unsigned long long v){ return Length(static_cast<double>(v) * 3.085677581491367e16); }
constexpr Length operator""_kpc (long double v)      { return Length(static_cast<double>(v) * 3.085677581491367e19); }
constexpr Length operator""_kpc (unsigned long long v){ return Length(static_cast<double>(v) * 3.085677581491367e19); }
constexpr Length operator""_Mpc (long double v)      { return Length(static_cast<double>(v) * 3.085677581491367e22); }
constexpr Length operator""_Mpc (unsigned long long v){ return Length(static_cast<double>(v) * 3.085677581491367e22); }

// --- Time (SI base: s) ---
constexpr Time operator""_s   (long double v)      { return Time(static_cast<double>(v)); }
constexpr Time operator""_s   (unsigned long long v){ return Time(static_cast<double>(v)); }
constexpr Time operator""_ms  (long double v)      { return Time(static_cast<double>(v) * 1e-3); }
constexpr Time operator""_ms  (unsigned long long v){ return Time(static_cast<double>(v) * 1e-3); }
constexpr Time operator""_us  (long double v)      { return Time(static_cast<double>(v) * 1e-6); }
constexpr Time operator""_us  (unsigned long long v){ return Time(static_cast<double>(v) * 1e-6); }
constexpr Time operator""_min (long double v)      { return Time(static_cast<double>(v) * 60.0); }
constexpr Time operator""_min (unsigned long long v){ return Time(static_cast<double>(v) * 60.0); }
constexpr Time operator""_hr  (long double v)      { return Time(static_cast<double>(v) * 3600.0); }
constexpr Time operator""_hr  (unsigned long long v){ return Time(static_cast<double>(v) * 3600.0); }
constexpr Time operator""_day (long double v)      { return Time(static_cast<double>(v) * 86400.0); }
constexpr Time operator""_day (unsigned long long v){ return Time(static_cast<double>(v) * 86400.0); }
constexpr Time operator""_yr  (long double v)      { return Time(static_cast<double>(v) * 31557600.0); }
constexpr Time operator""_yr  (unsigned long long v){ return Time(static_cast<double>(v) * 31557600.0); }

// --- Current (SI base: A) ---
constexpr Current operator""_A  (long double v)      { return Current(static_cast<double>(v)); }
constexpr Current operator""_A  (unsigned long long v){ return Current(static_cast<double>(v)); }
constexpr Current operator""_mA (long double v)      { return Current(static_cast<double>(v) * 1e-3); }
constexpr Current operator""_mA (unsigned long long v){ return Current(static_cast<double>(v) * 1e-3); }
constexpr Current operator""_uA (long double v)      { return Current(static_cast<double>(v) * 1e-6); }
constexpr Current operator""_uA (unsigned long long v){ return Current(static_cast<double>(v) * 1e-6); }
constexpr Current operator""_nA (long double v)      { return Current(static_cast<double>(v) * 1e-9); }
constexpr Current operator""_nA (unsigned long long v){ return Current(static_cast<double>(v) * 1e-9); }

// --- Temperature (SI base: K — absolute) ---
// NOTE: Affine offsets (°C, °F) are converted to Kelvin on construction.
constexpr Temperature operator""_K    (long double v)      { return Temperature(static_cast<double>(v)); }
constexpr Temperature operator""_K    (unsigned long long v){ return Temperature(static_cast<double>(v)); }
constexpr Temperature operator""_degC (long double v)      { return Temperature(static_cast<double>(v) + 273.15); }
constexpr Temperature operator""_degC (unsigned long long v){ return Temperature(static_cast<double>(v) + 273.15); }
constexpr Temperature operator""_degF (long double v)      { return Temperature(static_cast<double>((v - 32.0L) * 5.0L/9.0L + 273.15L)); }
constexpr Temperature operator""_degF (unsigned long long v){ return Temperature((static_cast<double>(v) - 32.0) * 5.0/9.0 + 273.15); }

// --- Amount (SI base: mol) ---
constexpr Amount operator""_mol  (long double v)      { return Amount(static_cast<double>(v)); }
constexpr Amount operator""_mol  (unsigned long long v){ return Amount(static_cast<double>(v)); }
constexpr Amount operator""_mmol (long double v)      { return Amount(static_cast<double>(v) * 1e-3); }
constexpr Amount operator""_mmol (unsigned long long v){ return Amount(static_cast<double>(v) * 1e-3); }

// --- Luminosity (SI base: cd) ---
constexpr Luminosity operator""_cd (long double v)      { return Luminosity(static_cast<double>(v)); }
constexpr Luminosity operator""_cd (unsigned long long v){ return Luminosity(static_cast<double>(v)); }

// =============================================================================
// UDLs — Derived and Non-SI Units (converted to SI on construction)
// =============================================================================

// --- Force (SI base: N = kg·m/s²) ---
constexpr Force operator""_N   (long double v)      { return Force(static_cast<double>(v)); }
constexpr Force operator""_N   (unsigned long long v){ return Force(static_cast<double>(v)); }
constexpr Force operator""_kN  (long double v)      { return Force(static_cast<double>(v) * 1e3); }
constexpr Force operator""_kN  (unsigned long long v){ return Force(static_cast<double>(v) * 1e3); }
constexpr Force operator""_lbf (long double v)      { return Force(static_cast<double>(v) * 4.4482216152605); }
constexpr Force operator""_lbf (unsigned long long v){ return Force(static_cast<double>(v) * 4.4482216152605); }

// --- Energy (SI base: J = kg·m²/s²) ---
constexpr Energy operator""_J    (long double v)      { return Energy(static_cast<double>(v)); }
constexpr Energy operator""_J    (unsigned long long v){ return Energy(static_cast<double>(v)); }
constexpr Energy operator""_kJ   (long double v)      { return Energy(static_cast<double>(v) * 1e3); }
constexpr Energy operator""_kJ   (unsigned long long v){ return Energy(static_cast<double>(v) * 1e3); }
constexpr Energy operator""_cal  (long double v)      { return Energy(static_cast<double>(v) * 4.184); }
constexpr Energy operator""_cal  (unsigned long long v){ return Energy(static_cast<double>(v) * 4.184); }
constexpr Energy operator""_kcal (long double v)      { return Energy(static_cast<double>(v) * 4184.0); }
constexpr Energy operator""_kcal (unsigned long long v){ return Energy(static_cast<double>(v) * 4184.0); }
constexpr Energy operator""_eV   (long double v)      { return Energy(static_cast<double>(v) * 1.602176634e-19); }
constexpr Energy operator""_eV   (unsigned long long v){ return Energy(static_cast<double>(v) * 1.602176634e-19); }
constexpr Energy operator""_meV  (long double v)      { return Energy(static_cast<double>(v) * 1.602176634e-22); }
constexpr Energy operator""_meV  (unsigned long long v){ return Energy(static_cast<double>(v) * 1.602176634e-22); }
constexpr Energy operator""_MeV  (long double v)      { return Energy(static_cast<double>(v) * 1.602176634e-13); }
constexpr Energy operator""_MeV  (unsigned long long v){ return Energy(static_cast<double>(v) * 1.602176634e-13); }
constexpr Energy operator""_GeV  (long double v)      { return Energy(static_cast<double>(v) * 1.602176634e-10); }
constexpr Energy operator""_GeV  (unsigned long long v){ return Energy(static_cast<double>(v) * 1.602176634e-10); }
constexpr Energy operator""_Wh   (long double v)      { return Energy(static_cast<double>(v) * 3600.0); }
constexpr Energy operator""_Wh   (unsigned long long v){ return Energy(static_cast<double>(v) * 3600.0); }
constexpr Energy operator""_kWh  (long double v)      { return Energy(static_cast<double>(v) * 3.6e6); }
constexpr Energy operator""_kWh  (unsigned long long v){ return Energy(static_cast<double>(v) * 3.6e6); }
constexpr Energy operator""_BTU  (long double v)      { return Energy(static_cast<double>(v) * 1055.05585262); }
constexpr Energy operator""_BTU  (unsigned long long v){ return Energy(static_cast<double>(v) * 1055.05585262); }

// --- Power (SI base: W = kg·m²/s³) ---
constexpr Power operator""_W  (long double v)      { return Power(static_cast<double>(v)); }
constexpr Power operator""_W  (unsigned long long v){ return Power(static_cast<double>(v)); }
constexpr Power operator""_kW (long double v)      { return Power(static_cast<double>(v) * 1e3); }
constexpr Power operator""_kW (unsigned long long v){ return Power(static_cast<double>(v) * 1e3); }
constexpr Power operator""_MW (long double v)      { return Power(static_cast<double>(v) * 1e6); }
constexpr Power operator""_MW (unsigned long long v){ return Power(static_cast<double>(v) * 1e6); }
constexpr Power operator""_hp (long double v)      { return Power(static_cast<double>(v) * 745.69987158227022); }
constexpr Power operator""_hp (unsigned long long v){ return Power(static_cast<double>(v) * 745.69987158227022); }

// --- Pressure (SI base: Pa = kg/(m·s²)) ---
constexpr Pressure operator""_Pa   (long double v)      { return Pressure(static_cast<double>(v)); }
constexpr Pressure operator""_Pa   (unsigned long long v){ return Pressure(static_cast<double>(v)); }
constexpr Pressure operator""_kPa  (long double v)      { return Pressure(static_cast<double>(v) * 1e3); }
constexpr Pressure operator""_kPa  (unsigned long long v){ return Pressure(static_cast<double>(v) * 1e3); }
constexpr Pressure operator""_MPa  (long double v)      { return Pressure(static_cast<double>(v) * 1e6); }
constexpr Pressure operator""_MPa  (unsigned long long v){ return Pressure(static_cast<double>(v) * 1e6); }
constexpr Pressure operator""_bar  (long double v)      { return Pressure(static_cast<double>(v) * 1e5); }
constexpr Pressure operator""_bar  (unsigned long long v){ return Pressure(static_cast<double>(v) * 1e5); }
constexpr Pressure operator""_atm  (long double v)      { return Pressure(static_cast<double>(v) * 101325.0); }
constexpr Pressure operator""_atm  (unsigned long long v){ return Pressure(static_cast<double>(v) * 101325.0); }
constexpr Pressure operator""_psi  (long double v)      { return Pressure(static_cast<double>(v) * 6894.757293168); }
constexpr Pressure operator""_psi  (unsigned long long v){ return Pressure(static_cast<double>(v) * 6894.757293168); }
constexpr Pressure operator""_torr (long double v)      { return Pressure(static_cast<double>(v) * (101325.0 / 760.0)); }
constexpr Pressure operator""_torr (unsigned long long v){ return Pressure(static_cast<double>(v) * (101325.0 / 760.0)); }
constexpr Pressure operator""_mmHg (long double v)      { return Pressure(static_cast<double>(v) * 133.322387415); }
constexpr Pressure operator""_mmHg (unsigned long long v){ return Pressure(static_cast<double>(v) * 133.322387415); }

// --- Frequency (SI base: Hz = 1/s) ---
constexpr Frequency operator""_Hz  (long double v)      { return Frequency(static_cast<double>(v)); }
constexpr Frequency operator""_Hz  (unsigned long long v){ return Frequency(static_cast<double>(v)); }
constexpr Frequency operator""_kHz (long double v)      { return Frequency(static_cast<double>(v) * 1e3); }
constexpr Frequency operator""_kHz (unsigned long long v){ return Frequency(static_cast<double>(v) * 1e3); }
constexpr Frequency operator""_MHz (long double v)      { return Frequency(static_cast<double>(v) * 1e6); }
constexpr Frequency operator""_MHz (unsigned long long v){ return Frequency(static_cast<double>(v) * 1e6); }
constexpr Frequency operator""_GHz (long double v)      { return Frequency(static_cast<double>(v) * 1e9); }
constexpr Frequency operator""_GHz (unsigned long long v){ return Frequency(static_cast<double>(v) * 1e9); }

// --- Volume (SI base: m³) ---
constexpr Volume operator""_L  (long double v)      { return Volume(static_cast<double>(v) * 1e-3); }
constexpr Volume operator""_L  (unsigned long long v){ return Volume(static_cast<double>(v) * 1e-3); }
constexpr Volume operator""_mL (long double v)      { return Volume(static_cast<double>(v) * 1e-6); }
constexpr Volume operator""_mL (unsigned long long v){ return Volume(static_cast<double>(v) * 1e-6); }

// --- Area ---
constexpr Area operator""_b  (long double v)      { return Area(static_cast<double>(v) * 1e-28); }  // barn
constexpr Area operator""_b  (unsigned long long v){ return Area(static_cast<double>(v) * 1e-28); }

// --- Velocity ---
constexpr Velocity operator""_kn (long double v)      { return Velocity(static_cast<double>(v) * (1852.0 / 3600.0)); }
constexpr Velocity operator""_kn (unsigned long long v){ return Velocity(static_cast<double>(v) * (1852.0 / 3600.0)); }

// --- Voltage ---
constexpr Voltage operator""_MV (long double v)      { return Voltage(static_cast<double>(v) * 1e6); }
constexpr Voltage operator""_MV (unsigned long long v){ return Voltage(static_cast<double>(v) * 1e6); }
constexpr Voltage operator""_kV (long double v)      { return Voltage(static_cast<double>(v) * 1e3); }
constexpr Voltage operator""_kV (unsigned long long v){ return Voltage(static_cast<double>(v) * 1e3); }
constexpr Voltage operator""_V  (long double v)      { return Voltage(static_cast<double>(v)); }
constexpr Voltage operator""_V  (unsigned long long v){ return Voltage(static_cast<double>(v)); }
constexpr Voltage operator""_mV (long double v)      { return Voltage(static_cast<double>(v) * 1e-3); }
constexpr Voltage operator""_mV (unsigned long long v){ return Voltage(static_cast<double>(v) * 1e-3); }
constexpr Voltage operator""_uV (long double v)      { return Voltage(static_cast<double>(v) * 1e-6); }
constexpr Voltage operator""_uV (unsigned long long v){ return Voltage(static_cast<double>(v) * 1e-6); }

// --- Charge (SI base: C = A·s) ---
constexpr Charge operator""_C  (long double v)      { return Charge(static_cast<double>(v)); }
constexpr Charge operator""_C  (unsigned long long v){ return Charge(static_cast<double>(v)); }
constexpr Charge operator""_mC (long double v)      { return Charge(static_cast<double>(v) * 1e-3); }
constexpr Charge operator""_mC (unsigned long long v){ return Charge(static_cast<double>(v) * 1e-3); }
constexpr Charge operator""_uC (long double v)      { return Charge(static_cast<double>(v) * 1e-6); }
constexpr Charge operator""_uC (unsigned long long v){ return Charge(static_cast<double>(v) * 1e-6); }
constexpr Charge operator""_nC (long double v)      { return Charge(static_cast<double>(v) * 1e-9); }
constexpr Charge operator""_nC (unsigned long long v){ return Charge(static_cast<double>(v) * 1e-9); }
constexpr Charge operator""_pC (long double v)      { return Charge(static_cast<double>(v) * 1e-12); }
constexpr Charge operator""_pC (unsigned long long v){ return Charge(static_cast<double>(v) * 1e-12); }

// --- Electromagnetism — named SI units with prefix ladder ---
constexpr MagneticFlux  operator""_Wb   (long double v)      { return MagneticFlux(static_cast<double>(v)); }
constexpr MagneticFlux  operator""_Wb   (unsigned long long v){ return MagneticFlux(static_cast<double>(v)); }
constexpr MagneticField operator""_T    (long double v)      { return MagneticField(static_cast<double>(v)); }
constexpr MagneticField operator""_T    (unsigned long long v){ return MagneticField(static_cast<double>(v)); }
constexpr Inductance    operator""_H    (long double v)      { return Inductance(static_cast<double>(v)); }
constexpr Inductance    operator""_H    (unsigned long long v){ return Inductance(static_cast<double>(v)); }
constexpr Inductance    operator""_mH   (long double v)      { return Inductance(static_cast<double>(v) * 1e-3); }
constexpr Inductance    operator""_mH   (unsigned long long v){ return Inductance(static_cast<double>(v) * 1e-3); }
constexpr Inductance    operator""_uH   (long double v)      { return Inductance(static_cast<double>(v) * 1e-6); }
constexpr Inductance    operator""_uH   (unsigned long long v){ return Inductance(static_cast<double>(v) * 1e-6); }
constexpr Inductance    operator""_nH   (long double v)      { return Inductance(static_cast<double>(v) * 1e-9); }
constexpr Inductance    operator""_nH   (unsigned long long v){ return Inductance(static_cast<double>(v) * 1e-9); }
constexpr Capacitance   operator""_F    (long double v)      { return Capacitance(static_cast<double>(v)); }
constexpr Capacitance   operator""_F    (unsigned long long v){ return Capacitance(static_cast<double>(v)); }
constexpr Capacitance   operator""_mF   (long double v)      { return Capacitance(static_cast<double>(v) * 1e-3); }
constexpr Capacitance   operator""_mF   (unsigned long long v){ return Capacitance(static_cast<double>(v) * 1e-3); }
constexpr Capacitance   operator""_uF   (long double v)      { return Capacitance(static_cast<double>(v) * 1e-6); }
constexpr Capacitance   operator""_uF   (unsigned long long v){ return Capacitance(static_cast<double>(v) * 1e-6); }
constexpr Capacitance   operator""_nF   (long double v)      { return Capacitance(static_cast<double>(v) * 1e-9); }
constexpr Capacitance   operator""_nF   (unsigned long long v){ return Capacitance(static_cast<double>(v) * 1e-9); }
constexpr Capacitance   operator""_pF   (long double v)      { return Capacitance(static_cast<double>(v) * 1e-12); }
constexpr Capacitance   operator""_pF   (unsigned long long v){ return Capacitance(static_cast<double>(v) * 1e-12); }
constexpr Resistance    operator""_Mohm (long double v)      { return Resistance(static_cast<double>(v) * 1e6); }
constexpr Resistance    operator""_Mohm (unsigned long long v){ return Resistance(static_cast<double>(v) * 1e6); }
constexpr Resistance    operator""_kohm (long double v)      { return Resistance(static_cast<double>(v) * 1e3); }
constexpr Resistance    operator""_kohm (unsigned long long v){ return Resistance(static_cast<double>(v) * 1e3); }
constexpr Resistance    operator""_ohm  (long double v)      { return Resistance(static_cast<double>(v)); }
constexpr Resistance    operator""_ohm  (unsigned long long v){ return Resistance(static_cast<double>(v)); }
constexpr Resistance    operator""_mohm (long double v)      { return Resistance(static_cast<double>(v) * 1e-3); }
constexpr Resistance    operator""_mohm (unsigned long long v){ return Resistance(static_cast<double>(v) * 1e-3); }
constexpr Conductance   operator""_S    (long double v)      { return Conductance(static_cast<double>(v)); }
constexpr Conductance   operator""_S    (unsigned long long v){ return Conductance(static_cast<double>(v)); }

// --- Radiation / Dosimetry ---
constexpr RadioactiveActivity operator""_Bq (long double v)      { return RadioactiveActivity(static_cast<double>(v)); }
constexpr RadioactiveActivity operator""_Bq (unsigned long long v){ return RadioactiveActivity(static_cast<double>(v)); }
constexpr RadioactiveActivity operator""_Ci (long double v)      { return RadioactiveActivity(static_cast<double>(v) * 3.7e10); }
constexpr RadioactiveActivity operator""_Ci (unsigned long long v){ return RadioactiveActivity(static_cast<double>(v) * 3.7e10); }
constexpr AbsorbedDose        operator""_Gy (long double v)      { return AbsorbedDose(static_cast<double>(v)); }
constexpr AbsorbedDose        operator""_Gy (unsigned long long v){ return AbsorbedDose(static_cast<double>(v)); }
constexpr AbsorbedDose        operator""_Sv (long double v)      { return AbsorbedDose(static_cast<double>(v)); }
constexpr AbsorbedDose        operator""_Sv (unsigned long long v){ return AbsorbedDose(static_cast<double>(v)); }

// --- Photometry ---
constexpr LuminousFlux operator""_lm (long double v)      { return LuminousFlux(static_cast<double>(v)); }
constexpr LuminousFlux operator""_lm (unsigned long long v){ return LuminousFlux(static_cast<double>(v)); }
constexpr Illuminance  operator""_lx (long double v)      { return Illuminance(static_cast<double>(v)); }
constexpr Illuminance  operator""_lx (unsigned long long v){ return Illuminance(static_cast<double>(v)); }

} // inline namespace si_literals
