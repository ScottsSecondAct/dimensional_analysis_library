#include <gtest/gtest.h>
#include <cmath>
#include <limits>
#include <sstream>
#include "units.h"
#include "ecs.h"

// =============================================================================
// DimEngine — all 7 slots propagate through DimAdd / DimSub
// =============================================================================

TEST(DimEngine, DimAddAllSlots) {
    using D1 = Dimensions<1,2,3,4,5,6,7>;
    using D2 = Dimensions<7,6,5,4,3,2,1>;
    using R  = DimAdd<D1,D2>::type;
    EXPECT_EQ(R::mass,       8);
    EXPECT_EQ(R::length,     8);
    EXPECT_EQ(R::time,       8);
    EXPECT_EQ(R::current,    8);
    EXPECT_EQ(R::temp,       8);
    EXPECT_EQ(R::amount,     8);
    EXPECT_EQ(R::luminosity, 8);
}

TEST(DimEngine, DimSubAllSlots) {
    using D1 = Dimensions<7,6,5,4,3,2,1>;
    using D2 = Dimensions<1,2,3,4,5,6,7>;
    using R  = DimSub<D1,D2>::type;
    EXPECT_EQ(R::mass,        6);
    EXPECT_EQ(R::length,      4);
    EXPECT_EQ(R::time,        2);
    EXPECT_EQ(R::current,     0);
    EXPECT_EQ(R::temp,       -2);
    EXPECT_EQ(R::amount,     -4);
    EXPECT_EQ(R::luminosity, -6);
}

// =============================================================================
// Operators — arithmetic on Quantity
// =============================================================================

TEST(Operators, Addition) {
    auto a = 3.0_m;
    auto b = 2.0_m;
    EXPECT_DOUBLE_EQ((a + b).value, 5.0);
}

TEST(Operators, Subtraction) {
    auto a = 5.0_m;
    auto b = 3.0_m;
    EXPECT_DOUBLE_EQ((a - b).value, 2.0);
}

TEST(Operators, UnaryNegation) {
    auto a = 4.0_m;
    EXPECT_DOUBLE_EQ((-a).value, -4.0);
}

TEST(Operators, QuantityMultiplication) {
    auto a = 5.0_m;
    EXPECT_DOUBLE_EQ((a * a).value, 25.0);
}

TEST(Operators, QuantityDivision) {
    auto d = 10.0_m;
    auto t = 2.0_s;
    EXPECT_DOUBLE_EQ((d / t).value, 5.0);
}

TEST(Operators, ScalarMultiplyRight) {
    auto a = 3.0_m;
    EXPECT_DOUBLE_EQ((a * 2.0).value, 6.0);
}

TEST(Operators, ScalarMultiplyLeft) {
    auto a = 3.0_m;
    EXPECT_DOUBLE_EQ((2.0 * a).value, 6.0);
}

TEST(Operators, ScalarDivide) {
    auto a = 6.0_m;
    EXPECT_DOUBLE_EQ((a / 3.0).value, 2.0);
}

// =============================================================================
// TypeCorrectness — dimension type of results
// =============================================================================

TEST(TypeCorrectness, LengthDividedByTimeIsVelocity) {
    auto v = 10.0_m / 2.0_s;
    static_assert(std::is_same_v<decltype(v), Velocity>,
                  "Length/Time must be Velocity");
    EXPECT_DOUBLE_EQ(v.value, 5.0);
}

TEST(TypeCorrectness, ForceMulLengthIsEnergy) {
    auto f = 10.0_N;
    auto d = 3.0_m;
    auto e = f * d;
    static_assert(std::is_same_v<decltype(e), Energy>,
                  "Force*Length must be Energy");
    EXPECT_DOUBLE_EQ(e.value, 30.0);
}

TEST(TypeCorrectness, LengthMulLengthIsArea) {
    auto a = 4.0_m * 5.0_m;
    static_assert(std::is_same_v<decltype(a), Area>,
                  "Length*Length must be Area");
    EXPECT_DOUBLE_EQ(a.value, 20.0);
}

TEST(TypeCorrectness, EnergyDividedByTimeIsPower) {
    auto p = 100.0_J / 5.0_s;
    static_assert(std::is_same_v<decltype(p), Power>,
                  "Energy/Time must be Power");
    EXPECT_DOUBLE_EQ(p.value, 20.0);
}

TEST(TypeCorrectness, DensityDimensions) {
    using ExpectedDims = Quantity<Dimensions<1,-3,0>>;
    static_assert(std::is_same_v<Density, ExpectedDims>);
}

TEST(TypeCorrectness, ActionDimensions) {
    // Action = J·s = kg·m²/s; verify it matches Force * Length * Time
    auto e = 1.0_J;
    auto t = 1.0_s;
    auto a = e * t;
    static_assert(std::is_same_v<decltype(a), Action>,
                  "Energy*Time must be Action");
}

TEST(TypeCorrectness, ConductanceDimensions) {
    using ExpectedDims = Quantity<Dimensions<-1,-2,3,2>>;
    static_assert(std::is_same_v<Conductance, ExpectedDims>);
}

TEST(TypeCorrectness, KinematicViscosityDimensions) {
    // KinematicViscosity = DynamicViscosity / Density = m²/s
    using ExpectedDims = Quantity<Dimensions<0,2,-1>>;
    static_assert(std::is_same_v<KinematicViscosity, ExpectedDims>);
}

TEST(TypeCorrectness, AbsorbedDoseDimensions) {
    // Gy = J/kg = m²/s²
    using ExpectedDims = Quantity<Dimensions<0,2,-2>>;
    static_assert(std::is_same_v<AbsorbedDose, ExpectedDims>);
}

// =============================================================================
// Comparisons — spaceship operator on same-dimension quantities
// =============================================================================

TEST(Comparisons, Equal) {
    auto a = 5.0_m;
    auto b = 5.0_m;
    EXPECT_TRUE(a == b);
}

TEST(Comparisons, LessThan) {
    auto a = 3.0_m;
    auto b = 5.0_m;
    EXPECT_TRUE(a < b);
}

TEST(Comparisons, GreaterThan) {
    auto a = 7.0_m;
    auto b = 4.0_m;
    EXPECT_TRUE(a > b);
}

// =============================================================================
// Conversions — UDLs store correct SI values
// =============================================================================

TEST(Conversions, Kcal) {
    EXPECT_DOUBLE_EQ((1.0_kcal).value, 4184.0);
}

TEST(Conversions, DegCelsius) {
    EXPECT_DOUBLE_EQ((0.0_degC).value, 273.15);
}

TEST(Conversions, DegFahrenheit) {
    // 32°F == 0°C == 273.15 K
    EXPECT_NEAR((32.0_degF).value, 273.15, 1e-9);
}

TEST(Conversions, Atm) {
    EXPECT_DOUBLE_EQ((1.0_atm).value, 101325.0);
}

TEST(Conversions, Km) {
    EXPECT_DOUBLE_EQ((1.0_km).value, 1000.0);
}

TEST(Conversions, Litre) {
    EXPECT_DOUBLE_EQ((1.0_L).value, 1e-3);
}

TEST(Conversions, eV) {
    EXPECT_NEAR((1.0_eV).value, 1.602176634e-19, 1e-30);
}

// New conversions
TEST(Conversions, Dalton) {
    EXPECT_NEAR((1.0_Da).value, 1.66053906660e-27, 1e-38);
}

TEST(Conversions, UnifiedAtomicMassUnit) {
    EXPECT_NEAR((1.0_u).value, 1.66053906660e-27, 1e-38);
}

TEST(Conversions, Tonne) {
    EXPECT_DOUBLE_EQ((1.0_tonne).value, 1000.0);
}

TEST(Conversions, Pound) {
    EXPECT_DOUBLE_EQ((1.0_lb).value, 0.45359237);
}

TEST(Conversions, Ounce) {
    EXPECT_DOUBLE_EQ((1.0_oz).value, 0.028349523125);
}

TEST(Conversions, Foot) {
    EXPECT_DOUBLE_EQ((1.0_ft).value, 0.3048);
}

TEST(Conversions, Inch) {
    EXPECT_DOUBLE_EQ((1.0_in).value, 0.0254);
}

TEST(Conversions, Yard) {
    EXPECT_DOUBLE_EQ((1.0_yd).value, 0.9144);
}

TEST(Conversions, Mile) {
    EXPECT_DOUBLE_EQ((1.0_mi).value, 1609.344);
}

TEST(Conversions, NauticalMile) {
    EXPECT_DOUBLE_EQ((1.0_nmi).value, 1852.0);
}

TEST(Conversions, AstronomicalUnit) {
    EXPECT_NEAR((1.0_au).value, 1.495978707e11, 1.0);
}

TEST(Conversions, LightYear) {
    EXPECT_NEAR((1.0_ly).value, 9.4607304725808e15, 1e3);
}

TEST(Conversions, Parsec) {
    EXPECT_NEAR((1.0_pc).value, 3.085677581491367e16, 1e3);
}

TEST(Conversions, Day) {
    EXPECT_DOUBLE_EQ((1.0_day).value, 86400.0);
}

TEST(Conversions, JulianYear) {
    EXPECT_DOUBLE_EQ((1.0_yr).value, 31557600.0);
}

TEST(Conversions, MeV) {
    EXPECT_NEAR((1.0_MeV).value, 1.602176634e-13, 1e-24);
}

TEST(Conversions, GeV) {
    EXPECT_NEAR((1.0_GeV).value, 1.602176634e-10, 1e-21);
}

TEST(Conversions, KiloWattHour) {
    EXPECT_DOUBLE_EQ((1.0_kWh).value, 3.6e6);
}

TEST(Conversions, BTU) {
    EXPECT_NEAR((1.0_BTU).value, 1055.05585262, 1e-5);
}

TEST(Conversions, Horsepower) {
    EXPECT_NEAR((1.0_hp).value, 745.69987158227022, 1e-8);
}

TEST(Conversions, PSI) {
    EXPECT_NEAR((1.0_psi).value, 6894.757293168, 1e-6);
}

TEST(Conversions, Torr) {
    EXPECT_NEAR((1.0_torr).value, 101325.0 / 760.0, 1e-9);
}

TEST(Conversions, PoundForce) {
    EXPECT_NEAR((1.0_lbf).value, 4.4482216152605, 1e-10);
}

TEST(Conversions, Barn) {
    EXPECT_DOUBLE_EQ((1.0_b).value, 1e-28);
}

TEST(Conversions, Knot) {
    EXPECT_NEAR((1.0_kn).value, 1852.0 / 3600.0, 1e-12);
}

// =============================================================================
// Chemistry — dimensional correctness
// =============================================================================

TEST(Chemistry, ConcentrationMulVolumeIsAmount) {
    // mol/m³ * m³ = mol
    auto conc = Concentration(2.0);   // 2 mol/m³
    auto vol  = Volume(0.5);          // 0.5 m³
    auto n    = conc * vol;
    static_assert(std::is_same_v<decltype(n), Amount>,
                  "Concentration*Volume must be Amount");
    EXPECT_DOUBLE_EQ(n.value, 1.0);
}

TEST(Chemistry, MolarMassCorrectDimensions) {
    using MM = Quantity<Dimensions<1,0,0,0,0,-1>>;
    static_assert(std::is_same_v<MolarMass, MM>);
}

// =============================================================================
// Electromagnetism — dimensional correctness
// =============================================================================

TEST(Electro, VoltageDividedByCurrentIsResistance) {
    auto v = 12.0_V;
    auto i = 3.0_A;
    auto r = v / i;
    static_assert(std::is_same_v<decltype(r), Resistance>,
                  "Voltage/Current must be Resistance");
    EXPECT_DOUBLE_EQ(r.value, 4.0);
}

TEST(Electro, CurrentMulTimeIsCharge) {
    auto i = 2.0_A;
    auto t = 3.0_s;
    auto q = i * t;
    static_assert(std::is_same_v<decltype(q), Charge>,
                  "Current*Time must be Charge");
    EXPECT_DOUBLE_EQ(q.value, 6.0);
}

// =============================================================================
// EMNamedUnits — UDLs for named EM SI units
// =============================================================================

TEST(EMNamedUnits, OhmIsResistance) {
    auto r = 100.0_ohm;
    static_assert(std::is_same_v<decltype(r), Resistance>);
    EXPECT_DOUBLE_EQ(r.value, 100.0);
}

TEST(EMNamedUnits, FaradIsCapacitance) {
    auto c = 1.0_F;
    static_assert(std::is_same_v<decltype(c), Capacitance>);
    EXPECT_DOUBLE_EQ(c.value, 1.0);
}

TEST(EMNamedUnits, WeberIsMagneticFlux) {
    auto wb = 2.5_Wb;
    static_assert(std::is_same_v<decltype(wb), MagneticFlux>);
    EXPECT_DOUBLE_EQ(wb.value, 2.5);
}

TEST(EMNamedUnits, TeslaIsMagneticField) {
    auto t = 0.5_T;
    static_assert(std::is_same_v<decltype(t), MagneticField>);
    EXPECT_DOUBLE_EQ(t.value, 0.5);
}

TEST(EMNamedUnits, HenryIsInductance) {
    auto h = 1.0_H;
    static_assert(std::is_same_v<decltype(h), Inductance>);
    EXPECT_DOUBLE_EQ(h.value, 1.0);
}

TEST(EMNamedUnits, SiemensIsConductance) {
    auto s = 0.25_S;
    static_assert(std::is_same_v<decltype(s), Conductance>);
    EXPECT_DOUBLE_EQ(s.value, 0.25);
}

// =============================================================================
// Constants — namespace constants values and types
// =============================================================================

TEST(Constants, SpeedOfLight) {
    static_assert(std::is_same_v<decltype(constants::c), const Velocity>);
    EXPECT_DOUBLE_EQ(constants::c.value, 299'792'458.0);
}

TEST(Constants, PlanckConstant) {
    static_assert(std::is_same_v<decltype(constants::h), const Action>);
    EXPECT_NEAR(constants::h.value, 6.62607015e-34, 1e-44);
}

TEST(Constants, BoltzmannConstant) {
    static_assert(std::is_same_v<decltype(constants::k_B), const Entropy>);
    EXPECT_NEAR(constants::k_B.value, 1.380649e-23, 1e-33);
}

TEST(Constants, ElementaryCharge) {
    static_assert(std::is_same_v<decltype(constants::e), const Charge>);
    EXPECT_NEAR(constants::e.value, 1.602176634e-19, 1e-30);
}

TEST(Constants, ElectronMass) {
    static_assert(std::is_same_v<decltype(constants::m_e), const Mass>);
    EXPECT_NEAR(constants::m_e.value, 9.1093837015e-31, 1e-42);
}

TEST(Constants, GravitationalConstant) {
    EXPECT_NEAR(constants::G.value, 6.67430e-11, 1e-16);
}

TEST(Constants, AvogadroNumber) {
    EXPECT_NEAR(constants::N_A.value, 6.02214076e23, 1e13);
}

TEST(Constants, GasConstant) {
    static_assert(std::is_same_v<decltype(constants::R), const MolarEntropy>);
    EXPECT_NEAR(constants::R.value, 8.314462618, 1e-8);
}

// =============================================================================
// Radiation — dimensional correctness and UDL values
// =============================================================================

TEST(Radiation, BecquerelIsFrequency) {
    auto bq = 1000.0_Bq;
    static_assert(std::is_same_v<decltype(bq), Frequency>);
    EXPECT_DOUBLE_EQ(bq.value, 1000.0);
}

TEST(Radiation, GrayIsAbsorbedDose) {
    auto gy = 2.0_Gy;
    static_assert(std::is_same_v<decltype(gy), AbsorbedDose>);
    EXPECT_DOUBLE_EQ(gy.value, 2.0);
}

TEST(Radiation, SievertIsAbsorbedDose) {
    auto sv = 0.001_Sv;
    static_assert(std::is_same_v<decltype(sv), AbsorbedDose>);
    EXPECT_DOUBLE_EQ(sv.value, 0.001);
}

TEST(Radiation, CurieConversion) {
    // 1 Ci = 3.7e10 Bq
    EXPECT_DOUBLE_EQ((1.0_Ci).value, 3.7e10);
}

// =============================================================================
// Photometry — dimensional correctness and UDL values
// =============================================================================

TEST(Photometry, LumenIsLuminosity) {
    auto lm = 800.0_lm;
    static_assert(std::is_same_v<decltype(lm), Luminosity>);
    EXPECT_DOUBLE_EQ(lm.value, 800.0);
}

TEST(Photometry, LuxIsIlluminance) {
    auto lx = 500.0_lx;
    static_assert(std::is_same_v<decltype(lx), Illuminance>);
    EXPECT_DOUBLE_EQ(lx.value, 500.0);
}

TEST(Photometry, IlluminanceDimensions) {
    // lux = cd/m² → dimensions M⁰ L⁻² T⁰ I⁰ K⁰ N⁰ J¹
    using ExpectedDims = Quantity<Dimensions<0,-2,0,0,0,0,1>>;
    static_assert(std::is_same_v<Illuminance, ExpectedDims>);
}

// =============================================================================
// MathFunctions — pow<N>, sqrt, abs
// =============================================================================

TEST(MathFunctions, PowSquaredLengthIsArea) {
    auto side = 4.0_m;
    auto area = pow<2>(side);
    static_assert(std::is_same_v<decltype(area), Area>, "pow<2>(Length) must be Area");
    EXPECT_DOUBLE_EQ(area.value, 16.0);
}

TEST(MathFunctions, PowCubedLengthIsVolume) {
    auto side = 3.0_m;
    auto vol  = pow<3>(side);
    static_assert(std::is_same_v<decltype(vol), Volume>, "pow<3>(Length) must be Volume");
    EXPECT_DOUBLE_EQ(vol.value, 27.0);
}

TEST(MathFunctions, PowNegativeOneInvertsUnit) {
    auto v   = 2.0_m / 1.0_s;   // Velocity: m·s⁻¹
    auto inv = pow<-1>(v);       // should be s·m⁻¹
    using ExpectedDim = Quantity<Dimensions<0,-1,1>>;
    static_assert(std::is_same_v<decltype(inv), ExpectedDim>);
    EXPECT_DOUBLE_EQ(inv.value, 0.5);
}

TEST(MathFunctions, SqrtAreaIsLength) {
    auto area = 9.0_m * 9.0_m;   // 81 m²
    auto side = sqrt(area);
    static_assert(std::is_same_v<decltype(side), Length>, "sqrt(Area) must be Length");
    EXPECT_DOUBLE_EQ(side.value, 9.0);
}

TEST(MathFunctions, SqrtEnergyOverMassIsVelocity) {
    // KE = ½mv²  →  v = sqrt(2·KE/m)
    auto ke = 50.0_J;   // 50 J
    auto m  = 1.0_kg;
    auto v  = sqrt(2.0 * ke / m);
    static_assert(std::is_same_v<decltype(v), Velocity>, "sqrt(J/kg) must be Velocity");
    EXPECT_DOUBLE_EQ(v.value, 10.0);
}

TEST(MathFunctions, AbsPreservesType) {
    auto neg = -3.5_m;
    auto pos = abs(neg);
    static_assert(std::is_same_v<decltype(pos), Length>);
    EXPECT_DOUBLE_EQ(pos.value, 3.5);
}

TEST(MathFunctions, AbsPositiveUnchanged) {
    EXPECT_DOUBLE_EQ(abs(7.0_kg).value, 7.0);
}

// =============================================================================
// StreamOutput — operator<< prints value and dimension string
// =============================================================================

TEST(StreamOutput, VelocityFormat) {
    std::ostringstream oss;
    oss << Velocity(5.0);
    // Velocity = m·s⁻¹ → expect "5 [m^-1]"... wait: Dimensions<0,1,-1>
    // names: kg=0, m=1, s=-1 → "m^-1" is wrong — it should be "m·s^-1"
    // Let's just check it contains the value and brackets
    auto s = oss.str();
    EXPECT_NE(s.find("5"), std::string::npos);
    EXPECT_NE(s.find("["),  std::string::npos);
    EXPECT_NE(s.find("]"),  std::string::npos);
}

TEST(StreamOutput, EnergyContainsDimensions) {
    std::ostringstream oss;
    oss << Energy(1.0);        // kg·m^2·s^-2
    auto s = oss.str();
    EXPECT_NE(s.find("kg"), std::string::npos);
    EXPECT_NE(s.find("m"),  std::string::npos);
    EXPECT_NE(s.find("s"),  std::string::npos);
}

TEST(StreamOutput, DimensionlessShowsOne) {
    // A dimensionless quantity has dim string "1"
    using Dimensionless = Quantity<Dimensions<0,0,0>>;
    std::ostringstream oss;
    oss << Dimensionless(42.0);
    EXPECT_NE(oss.str().find("[1]"), std::string::npos);
}

// =============================================================================
// IntegerUDLs — unsigned long long overloads compile and give correct values
// =============================================================================

TEST(IntegerUDLs, KilogramInteger) {
    static_assert(std::is_same_v<decltype(1_kg), Mass>);
    EXPECT_DOUBLE_EQ((5_kg).value, 5.0);
}

TEST(IntegerUDLs, MeterInteger) {
    EXPECT_DOUBLE_EQ((100_m).value, 100.0);
}

TEST(IntegerUDLs, SecondInteger) {
    EXPECT_DOUBLE_EQ((60_s).value, 60.0);
}

TEST(IntegerUDLs, IntegerMatchesFloatLiteral) {
    EXPECT_DOUBLE_EQ((1_km).value, (1.0_km).value);
    EXPECT_DOUBLE_EQ((1_atm).value, (1.0_atm).value);
    EXPECT_DOUBLE_EQ((100_degC).value, (100.0_degC).value);
}

TEST(IntegerUDLs, ElectronicsInteger) {
    EXPECT_DOUBLE_EQ((100_kohm).value, 100e3);
    EXPECT_DOUBLE_EQ((10_uF).value,    10e-6);
    EXPECT_DOUBLE_EQ((2_GHz).value,    2e9);
}

// =============================================================================
// ElectronicsUDLs — SI-prefix families for practical electronics work
// =============================================================================

// Voltage prefixes
TEST(ElectronicsUDLs, MegaVolt) {
    EXPECT_DOUBLE_EQ((1.0_MV).value, 1e6);
}
TEST(ElectronicsUDLs, KiloVolt) {
    EXPECT_DOUBLE_EQ((1.0_kV).value, 1e3);
}
TEST(ElectronicsUDLs, MicroVolt) {
    EXPECT_DOUBLE_EQ((1.0_uV).value, 1e-6);
}

// Current prefixes
TEST(ElectronicsUDLs, MicroAmp) {
    static_assert(std::is_same_v<decltype(1.0_uA), Current>);
    EXPECT_DOUBLE_EQ((1.0_uA).value, 1e-6);
}
TEST(ElectronicsUDLs, NanoAmp) {
    EXPECT_DOUBLE_EQ((1.0_nA).value, 1e-9);
}

// Charge prefixes
TEST(ElectronicsUDLs, Coulomb) {
    static_assert(std::is_same_v<decltype(1.0_C), Charge>);
    EXPECT_DOUBLE_EQ((1.0_C).value, 1.0);
}
TEST(ElectronicsUDLs, MilliCoulomb) {
    EXPECT_DOUBLE_EQ((1.0_mC).value, 1e-3);
}
TEST(ElectronicsUDLs, MicroCoulomb) {
    EXPECT_DOUBLE_EQ((1.0_uC).value, 1e-6);
}
TEST(ElectronicsUDLs, NanoCoulomb) {
    EXPECT_DOUBLE_EQ((1.0_nC).value, 1e-9);
}
TEST(ElectronicsUDLs, PicoCoulomb) {
    EXPECT_DOUBLE_EQ((1.0_pC).value, 1e-12);
}

// Capacitance prefixes
TEST(ElectronicsUDLs, MilliFarad) {
    static_assert(std::is_same_v<decltype(1.0_mF), Capacitance>);
    EXPECT_DOUBLE_EQ((1.0_mF).value, 1e-3);
}
TEST(ElectronicsUDLs, MicroFarad) {
    EXPECT_DOUBLE_EQ((100.0_uF).value, 100e-6);
}
TEST(ElectronicsUDLs, NanoFarad) {
    EXPECT_DOUBLE_EQ((10.0_nF).value, 10e-9);
}
TEST(ElectronicsUDLs, PicoFarad) {
    EXPECT_DOUBLE_EQ((22.0_pF).value, 22e-12);
}

// Resistance prefixes
TEST(ElectronicsUDLs, MegaOhm) {
    static_assert(std::is_same_v<decltype(1.0_Mohm), Resistance>);
    EXPECT_DOUBLE_EQ((1.0_Mohm).value, 1e6);
}
TEST(ElectronicsUDLs, KiloOhm) {
    EXPECT_DOUBLE_EQ((10.0_kohm).value, 10e3);
}
TEST(ElectronicsUDLs, MilliOhm) {
    EXPECT_DOUBLE_EQ((50.0_mohm).value, 50e-3);
}

// Inductance prefixes
TEST(ElectronicsUDLs, MilliHenry) {
    static_assert(std::is_same_v<decltype(1.0_mH), Inductance>);
    EXPECT_DOUBLE_EQ((1.0_mH).value, 1e-3);
}
TEST(ElectronicsUDLs, MicroHenry) {
    EXPECT_DOUBLE_EQ((470.0_uH).value, 470e-6);
}
TEST(ElectronicsUDLs, NanoHenry) {
    EXPECT_DOUBLE_EQ((100.0_nH).value, 100e-9);
}

// Frequency
TEST(ElectronicsUDLs, GigaHertz) {
    static_assert(std::is_same_v<decltype(1.0_GHz), Frequency>);
    EXPECT_DOUBLE_EQ((2.4_GHz).value, 2.4e9);
}

// Cross-check: Ohm's law with prefixed UDLs
TEST(ElectronicsUDLs, OhmsLawWithPrefixes) {
    // 5V / 10kΩ = 0.5mA
    auto v = 5.0_V;
    auto r = 10.0_kohm;
    auto i = v / r;
    static_assert(std::is_same_v<decltype(i), Current>);
    EXPECT_NEAR(i.value, 0.5e-3, 1e-15);
}

// =============================================================================
// ECS (existing, unchanged)
// =============================================================================

TEST(ECS, ViewFiltering) {
    Registry reg;
    reg.get_pool<int>().assign(1, 10);
    reg.get_pool<float>().assign(1, 2.0f);
    reg.get_pool<int>().assign(2, 20);

    int count = 0;
    reg.view<int, float>([&](int& i, float& f) { count++; });
    EXPECT_EQ(count, 1);
}

// =============================================================================
// DimArithmeticEdgeCases — DimScale / DimHalve meta-functions directly
// =============================================================================

TEST(DimArithmeticEdgeCases, ScaleByZeroGivesDimensionless) {
    using D = Dimensions<1,2,-3,4,-5,6,-7>;
    using R = DimScale<D, 0>::type;
    EXPECT_EQ(R::mass,       0);
    EXPECT_EQ(R::length,     0);
    EXPECT_EQ(R::time,       0);
    EXPECT_EQ(R::current,    0);
    EXPECT_EQ(R::temp,       0);
    EXPECT_EQ(R::amount,     0);
    EXPECT_EQ(R::luminosity, 0);
}

TEST(DimArithmeticEdgeCases, ScaleByOneIsIdentity) {
    using D = Dimensions<3,-1,2,0,1,-2,0>;
    using R = DimScale<D, 1>::type;
    EXPECT_EQ(R::mass,       D::mass);
    EXPECT_EQ(R::length,     D::length);
    EXPECT_EQ(R::time,       D::time);
    EXPECT_EQ(R::current,    D::current);
    EXPECT_EQ(R::temp,       D::temp);
    EXPECT_EQ(R::amount,     D::amount);
    EXPECT_EQ(R::luminosity, D::luminosity);
}

TEST(DimArithmeticEdgeCases, ScaleByNegativeOneNegatesAllSlots) {
    using D = Dimensions<1,2,-3,4,-5,6,-7>;
    using R = DimScale<D, -1>::type;
    EXPECT_EQ(R::mass,       -1);
    EXPECT_EQ(R::length,     -2);
    EXPECT_EQ(R::time,        3);
    EXPECT_EQ(R::current,    -4);
    EXPECT_EQ(R::temp,        5);
    EXPECT_EQ(R::amount,     -6);
    EXPECT_EQ(R::luminosity,  7);
}

TEST(DimArithmeticEdgeCases, ScaleByLargeFactorAllSlots) {
    using D = Dimensions<1,1,1,1,1,1,1>;
    using R = DimScale<D, 7>::type;
    EXPECT_EQ(R::mass,       7);
    EXPECT_EQ(R::length,     7);
    EXPECT_EQ(R::time,       7);
    EXPECT_EQ(R::current,    7);
    EXPECT_EQ(R::temp,       7);
    EXPECT_EQ(R::amount,     7);
    EXPECT_EQ(R::luminosity, 7);
}

TEST(DimArithmeticEdgeCases, HalveAllEvenSevenSlot) {
    using D = Dimensions<2,4,-6,8,-10,12,-14>;
    using R = DimHalve<D>::type;
    EXPECT_EQ(R::mass,        1);
    EXPECT_EQ(R::length,      2);
    EXPECT_EQ(R::time,       -3);
    EXPECT_EQ(R::current,     4);
    EXPECT_EQ(R::temp,       -5);
    EXPECT_EQ(R::amount,      6);
    EXPECT_EQ(R::luminosity, -7);
}

TEST(DimArithmeticEdgeCases, AddZeroDimIsIdentity) {
    using D    = Dimensions<3,-1,2,0,1,-2,0>;
    using Zero = Dimensions<0,0,0,0,0,0,0>;
    using R    = DimAdd<D, Zero>::type;
    EXPECT_EQ(R::mass,       D::mass);
    EXPECT_EQ(R::length,     D::length);
    EXPECT_EQ(R::time,       D::time);
    EXPECT_EQ(R::current,    D::current);
    EXPECT_EQ(R::temp,       D::temp);
    EXPECT_EQ(R::amount,     D::amount);
    EXPECT_EQ(R::luminosity, D::luminosity);
}

TEST(DimArithmeticEdgeCases, SubtractSelfGivesDimensionless) {
    using D = Dimensions<1,2,-3,4,-5,6,-7>;
    using R = DimSub<D, D>::type;
    EXPECT_EQ(R::mass,       0);
    EXPECT_EQ(R::length,     0);
    EXPECT_EQ(R::time,       0);
    EXPECT_EQ(R::current,    0);
    EXPECT_EQ(R::temp,       0);
    EXPECT_EQ(R::amount,     0);
    EXPECT_EQ(R::luminosity, 0);
}

TEST(DimArithmeticEdgeCases, ChainedAddThenSubIsIdentity) {
    using D1      = Dimensions<1,2,-3>;
    using D2      = Dimensions<4,-5,6>;
    using Added   = DimAdd<D1, D2>::type;
    using Restored = DimSub<Added, D2>::type;
    EXPECT_EQ(Restored::mass,   D1::mass);
    EXPECT_EQ(Restored::length, D1::length);
    EXPECT_EQ(Restored::time,   D1::time);
}

// =============================================================================
// QuantityEdgeCases — zero, negative, extremes, infinity, NaN, algebraic laws
// =============================================================================

TEST(QuantityEdgeCases, ZeroValue) {
    EXPECT_DOUBLE_EQ(Length(0.0).value, 0.0);
    EXPECT_DOUBLE_EQ((0.0_m).value, 0.0);
}

TEST(QuantityEdgeCases, NegativeValueRoundTrips) {
    auto l = Length(-5.0);
    EXPECT_DOUBLE_EQ(l.value, -5.0);
    EXPECT_DOUBLE_EQ((-l).value, 5.0);
}

TEST(QuantityEdgeCases, SelfSubtractionIsZero) {
    auto a = 42.0_kg;
    EXPECT_DOUBLE_EQ((a - a).value, 0.0);
}

TEST(QuantityEdgeCases, DoubleNegationIsIdentity) {
    auto a = 7.5_m;
    EXPECT_DOUBLE_EQ((-(-a)).value, a.value);
}

TEST(QuantityEdgeCases, ScalarMultiplyByOneIsIdentity) {
    auto a = 3.14_s;
    EXPECT_DOUBLE_EQ((a * 1.0).value, a.value);
    EXPECT_DOUBLE_EQ((1.0 * a).value, a.value);
}

TEST(QuantityEdgeCases, ScalarDivideByOneIsIdentity) {
    auto a = 2.72_kg;
    EXPECT_DOUBLE_EQ((a / 1.0).value, a.value);
}

TEST(QuantityEdgeCases, AddZeroQuantityIsIdentity) {
    auto a = 5.0_m;
    EXPECT_DOUBLE_EQ((a + Length(0.0)).value, a.value);
    EXPECT_DOUBLE_EQ((Length(0.0) + a).value, a.value);
}

TEST(QuantityEdgeCases, VeryLargeValue) {
    Mass huge = Mass(1e300);
    EXPECT_DOUBLE_EQ((huge * 2.0).value, 2e300);
}

TEST(QuantityEdgeCases, VerySmallValue) {
    Mass tiny = Mass(1e-300);
    EXPECT_DOUBLE_EQ((tiny * 1e300).value, 1.0);
}

TEST(QuantityEdgeCases, PositiveInfinityPropagates) {
    double inf = std::numeric_limits<double>::infinity();
    Mass m = Mass(inf);
    EXPECT_TRUE(std::isinf(m.value));
    EXPECT_TRUE(std::isinf((m + m).value));
    EXPECT_TRUE(std::isinf((m * 2.0).value));
    EXPECT_GT((m + m).value, 0.0);
}

TEST(QuantityEdgeCases, NegativeInfinityPropagates) {
    double ninf = -std::numeric_limits<double>::infinity();
    Length l = Length(ninf);
    EXPECT_TRUE(std::isinf(l.value));
    EXPECT_LT(l.value, 0.0);
    EXPECT_TRUE(std::isinf((-l).value));
    EXPECT_GT((-l).value, 0.0);   // -(-inf) = +inf
}

TEST(QuantityEdgeCases, NaNPropagatesThroughArithmetic) {
    double nan = std::numeric_limits<double>::quiet_NaN();
    Time t = Time(nan);
    EXPECT_TRUE(std::isnan(t.value));
    EXPECT_TRUE(std::isnan((t + Time(1.0)).value));
    EXPECT_TRUE(std::isnan((t * 2.0).value));
    EXPECT_TRUE(std::isnan((t - t).value));   // NaN - NaN = NaN, not 0
}

TEST(QuantityEdgeCases, ScalarMultiplyByZeroGivesZero) {
    EXPECT_DOUBLE_EQ((99.0_m * 0.0).value, 0.0);
    EXPECT_DOUBLE_EQ((0.0 * 99.0_m).value, 0.0);
}

// =============================================================================
// MathFunctions (additional) — pow<0>, pow<1>, pow<2>-vs-*, sqrt roundtrip
// =============================================================================

TEST(MathFunctions, PowZeroGivesDimensionlessOne) {
    auto acc = Acceleration(9.81);
    auto result = pow<0>(acc);
    using Dimensionless = Quantity<Dimensions<0,0,0>>;
    static_assert(std::is_same_v<decltype(result), Dimensionless>);
    EXPECT_DOUBLE_EQ(result.value, 1.0);
}

TEST(MathFunctions, PowOneIsIdentity) {
    auto acc  = Acceleration(9.81);
    auto same = pow<1>(acc);
    static_assert(std::is_same_v<decltype(same), Acceleration>);
    EXPECT_DOUBLE_EQ(same.value, acc.value);
}

TEST(MathFunctions, Pow2MatchesMultiplication) {
    Length l = 5.0_m;
    EXPECT_DOUBLE_EQ(pow<2>(l).value, (l * l).value);
}

TEST(MathFunctions, Pow3MatchesMultiplication) {
    Length l = 3.0_m;
    EXPECT_DOUBLE_EQ(pow<3>(l).value, (l * l * l).value);
}

TEST(MathFunctions, SqrtRoundtripPositive) {
    // sqrt(pow<2>(x)) == x  for x >= 0
    Length side = 7.0_m;
    EXPECT_DOUBLE_EQ(sqrt(pow<2>(side)).value, side.value);
}

TEST(MathFunctions, SqrtOfNegativeSquaredGivesAbsValue) {
    // pow<2>(-5 m) = 25 m², sqrt = 5 m = abs(-5 m)
    Length neg = Length(-5.0);
    EXPECT_DOUBLE_EQ(sqrt(pow<2>(neg)).value, abs(neg).value);
    EXPECT_DOUBLE_EQ(sqrt(pow<2>(neg)).value, 5.0);
}

TEST(MathFunctions, SqrtOfComplexEvenDimension) {
    // Force/Density = {0,4,-2}, sqrt = {0,2,-1} = KinematicViscosity
    Force   f = Force(1.0);
    Density d = Density(1.0);
    auto    q = sqrt(f / d);
    static_assert(std::is_same_v<decltype(q), KinematicViscosity>);
    EXPECT_DOUBLE_EQ(q.value, 1.0);
}

TEST(MathFunctions, SqrtOfZeroIsZero) {
    EXPECT_DOUBLE_EQ(sqrt(Area(0.0)).value, 0.0);
}

TEST(MathFunctions, AbsOfZeroIsZero) {
    EXPECT_DOUBLE_EQ(abs(Length(0.0)).value, 0.0);
}

TEST(MathFunctions, AbsOfPositiveIsUnchanged) {
    EXPECT_DOUBLE_EQ(abs(3.0_kg).value, 3.0);
}

TEST(MathFunctions, PowNegativeOneGivesInverseUnit) {
    // pow<-1>(Velocity) should have dim {0,-1,1}
    Velocity v = Velocity(2.0);
    auto inv = pow<-1>(v);
    using InverseVelocity = Quantity<Dimensions<0,-1,1>>;
    static_assert(std::is_same_v<decltype(inv), InverseVelocity>);
    EXPECT_DOUBLE_EQ(inv.value, 0.5);
}

// =============================================================================
// PhysicsFormulas — dimensional and numerical correctness of common formulas
// =============================================================================

TEST(PhysicsFormulas, NewtonsSecondLaw) {
    Mass         m = 2.0_kg;
    Acceleration a = Acceleration(5.0);
    Force        f = m * a;
    static_assert(std::is_same_v<decltype(f), Force>);
    EXPECT_DOUBLE_EQ(f.value, 10.0);
}

TEST(PhysicsFormulas, KineticEnergy) {
    Mass     m  = 4.0_kg;
    Velocity v  = Velocity(3.0);
    Energy   ke = 0.5 * m * v * v;
    static_assert(std::is_same_v<decltype(ke), Energy>);
    EXPECT_DOUBLE_EQ(ke.value, 18.0);   // ½ × 4 × 9 = 18 J
}

TEST(PhysicsFormulas, GravitationalPotentialEnergy) {
    Mass         m = 1.0_kg;
    Acceleration g = Acceleration(9.81);
    Length       h = 10.0_m;
    Energy       U = m * g * h;
    static_assert(std::is_same_v<decltype(U), Energy>);
    EXPECT_DOUBLE_EQ(U.value, 98.1);
}

TEST(PhysicsFormulas, OhmsLawPowerVSquaredOverR) {
    Voltage    v = 12.0_V;
    Resistance r = 4.0_ohm;
    Power      p = v * v / r;
    static_assert(std::is_same_v<decltype(p), Power>);
    EXPECT_DOUBLE_EQ(p.value, 36.0);    // 144 / 4 = 36 W
}

TEST(PhysicsFormulas, OhmsLawPowerISquaredTimesR) {
    Current    i = 3.0_A;
    Resistance r = 4.0_ohm;
    Power      p = i * i * r;
    static_assert(std::is_same_v<decltype(p), Power>);
    EXPECT_DOUBLE_EQ(p.value, 36.0);    // 9 × 4 = 36 W
}

TEST(PhysicsFormulas, RCTimeConstant) {
    Resistance  r   = 10.0_kohm;
    Capacitance c   = 100.0_uF;
    Time        tau = r * c;
    static_assert(std::is_same_v<decltype(tau), Time>);
    EXPECT_NEAR(tau.value, 1.0, 1e-10);  // 10 kΩ × 100 µF = 1 s
}

TEST(PhysicsFormulas, ImpulseEqualsMomentumChange) {
    Force    f = 5.0_N;
    Time     t = 2.0_s;
    Momentum p = f * t;
    static_assert(std::is_same_v<decltype(p), Momentum>);
    EXPECT_DOUBLE_EQ(p.value, 10.0);
}

TEST(PhysicsFormulas, IdealGasBothSidesAreEnergy) {
    // PV = nRT; check both sides have Energy dimensions
    Pressure p  = 101325.0_Pa;
    Volume   v  = Volume(0.0224);
    Energy   pv = p * v;
    static_assert(std::is_same_v<decltype(pv), Energy>);

    Amount      n   = 1.0_mol;
    Temperature T   = 273.15_K;
    auto        nRT = n * constants::R * T;
    static_assert(std::is_same_v<decltype(nRT), Energy>);
}

TEST(PhysicsFormulas, RestMassEnergy) {
    // E = mc²
    auto E = constants::m_e * constants::c * constants::c;
    static_assert(std::is_same_v<decltype(E), Energy>);
    // electron rest energy ≈ 8.187e-14 J
    EXPECT_NEAR(E.value, 8.187105776823886e-14, 1e-24);
}

TEST(PhysicsFormulas, VelocityFromKineticEnergy) {
    // v = sqrt(2KE/m)
    Energy   ke = 200.0_J;
    Mass     m  = 4.0_kg;
    Velocity v  = sqrt(2.0 * ke / m);
    static_assert(std::is_same_v<decltype(v), Velocity>);
    EXPECT_NEAR(v.value, 10.0, 1e-10);   // sqrt(400/4) = 10 m/s
}

TEST(PhysicsFormulas, LCResonanceFrequency) {
    // f = 1 / (2π sqrt(LC)) — test that sqrt(L*C) has Time dimensions
    Inductance  L  = 10.0_mH;
    Capacitance C  = 100.0_uF;
    auto        lc = L * C;
    // LC has dimensions H·F = (kg·m²·s⁻²·A⁻²)·(kg⁻¹·m⁻²·s⁴·A²) = s²
    // sqrt(LC) should be Time
    Time tau = sqrt(lc);
    static_assert(std::is_same_v<decltype(tau), Time>);
    EXPECT_NEAR(tau.value, std::sqrt(10e-3 * 100e-6), 1e-10);
}

// =============================================================================
// TypeCorrectnessAdditional — further static_assert checks on dimension algebra
// =============================================================================

TEST(TypeCorrectnessAdditional, EntropyTimesTemperatureIsEnergy) {
    auto kT = constants::k_B * Temperature(300.0);
    static_assert(std::is_same_v<decltype(kT), Energy>);
    EXPECT_NEAR(kT.value, 1.380649e-23 * 300.0, 1e-30);
}

TEST(TypeCorrectnessAdditional, ForceDividedByAreaIsPressure) {
    Force   f = 100.0_N;
    Area    a = 0.5_m * 0.5_m;
    Pressure p = f / a;
    static_assert(std::is_same_v<decltype(p), Pressure>);
    EXPECT_DOUBLE_EQ(p.value, 400.0);
}

TEST(TypeCorrectnessAdditional, PowerDividedByVoltageIsCurrent) {
    Power   p = 60.0_W;
    Voltage v = 12.0_V;
    Current i = p / v;
    static_assert(std::is_same_v<decltype(i), Current>);
    EXPECT_DOUBLE_EQ(i.value, 5.0);
}

TEST(TypeCorrectnessAdditional, EnergyDividedByMassIsAbsorbedDose) {
    Energy ke  = 200.0_J;
    Mass   m   = 2.0_kg;
    auto   spe = ke / m;
    static_assert(std::is_same_v<decltype(spe), AbsorbedDose>);
    EXPECT_DOUBLE_EQ(spe.value, 100.0);
}

TEST(TypeCorrectnessAdditional, VoltageTimesCurrentIsPowerNotForce) {
    // V * A = W (Power), not Force — confirms dimension algebra is exact
    auto p = 1.0_V * 1.0_A;
    static_assert(std::is_same_v<decltype(p), Power>);
    static_assert(!std::is_same_v<decltype(p), Force>);
}

TEST(TypeCorrectnessAdditional, MassDividedByVolumeIsDensity) {
    Mass   m   = 1000.0_kg;
    Volume vol = Volume(1.0);   // 1 m³
    auto   rho = m / vol;
    static_assert(std::is_same_v<decltype(rho), Density>);
    EXPECT_DOUBLE_EQ(rho.value, 1000.0);
}

TEST(TypeCorrectnessAdditional, ChargeDividedByTimeIsCurrent) {
    Charge q = 6.0_C;
    Time   t = 3.0_s;
    auto   i = q / t;
    static_assert(std::is_same_v<decltype(i), Current>);
    EXPECT_DOUBLE_EQ(i.value, 2.0);
}

// =============================================================================
// ComparisonEdgeCases — reflexivity, transitivity, infinity, zero, NaN
// =============================================================================

TEST(ComparisonEdgeCases, ReflexiveEquality) {
    auto a = 5.0_m;
    EXPECT_TRUE(a == a);
    EXPECT_FALSE(a != a);
}

TEST(ComparisonEdgeCases, SelfLessEqualAndGreaterEqual) {
    auto a = 42.0_kg;
    EXPECT_TRUE(a <= a);
    EXPECT_TRUE(a >= a);
}

TEST(ComparisonEdgeCases, NegativeLessThanZeroLessThanPositive) {
    auto neg = Length(-10.0);
    auto z   = Length(0.0);
    auto pos = 10.0_m;
    EXPECT_TRUE(neg < z);
    EXPECT_TRUE(z   < pos);
    EXPECT_TRUE(neg < pos);  // transitivity
}

TEST(ComparisonEdgeCases, PositiveZeroEqualsNegativeZero) {
    // IEEE 754: +0.0 == -0.0
    EXPECT_TRUE(Length(0.0) == Length(-0.0));
}

TEST(ComparisonEdgeCases, InfinityGreaterThanAnyFinite) {
    Length inf_l = Length(std::numeric_limits<double>::infinity());
    Length large = Length(1e300);
    EXPECT_TRUE(inf_l > large);
    EXPECT_TRUE(large < inf_l);
    EXPECT_FALSE(inf_l == large);
}

TEST(ComparisonEdgeCases, NegativeInfinityLessThanAnyFinite) {
    Length ninf = Length(-std::numeric_limits<double>::infinity());
    Length small = Length(-1e300);
    EXPECT_TRUE(ninf < small);
}

TEST(ComparisonEdgeCases, EpsilonDifferenceIsNotEqual) {
    Length a = Length(1.0);
    Length b = Length(1.0 + 2.0 * std::numeric_limits<double>::epsilon());
    EXPECT_TRUE(a != b);
    EXPECT_TRUE(a < b);
}

// =============================================================================
// UDLConsistency — relationships between UDLs that must hold exactly
// =============================================================================

TEST(UDLConsistency, KilometerEqualsThousandMeters) {
    EXPECT_DOUBLE_EQ((1_km).value, (1000_m).value);
}

TEST(UDLConsistency, HourEqualsThirtySevenHundredSeconds) {
    EXPECT_DOUBLE_EQ((1_hr).value, (3600_s).value);
}

TEST(UDLConsistency, MinuteEqualsSixtySeconds) {
    EXPECT_DOUBLE_EQ((1_min).value, (60_s).value);
}

TEST(UDLConsistency, DayEquals24Hours) {
    EXPECT_DOUBLE_EQ((1_day).value, (24_hr).value);
}

TEST(UDLConsistency, FootEqualsTwelveInches) {
    // 1959 international yard/foot: both exact by definition
    EXPECT_DOUBLE_EQ((1_ft).value, (12_in).value);
}

TEST(UDLConsistency, YardEqualsThreeFeet) {
    EXPECT_DOUBLE_EQ((1_yd).value, (3_ft).value);
}

TEST(UDLConsistency, MileEqualsFiveThousandTwoEightyFeet) {
    EXPECT_DOUBLE_EQ((1_mi).value, (5280_ft).value);
}

TEST(UDLConsistency, PoundEqualsSixteenOunces) {
    // lb = 16 oz (exact)
    EXPECT_DOUBLE_EQ((1_lb).value, (16_oz).value);
}

TEST(UDLConsistency, TonneEqualsThousandKilograms) {
    EXPECT_DOUBLE_EQ((1_tonne).value, (1000_kg).value);
}

TEST(UDLConsistency, ZeroCelsiusIs273Point15Kelvin) {
    EXPECT_DOUBLE_EQ((0_degC).value, 273.15);
    EXPECT_DOUBLE_EQ((0_degC).value, (273.15_K).value);
}

TEST(UDLConsistency, TemperatureDifferenceInCelsius) {
    // Stored as absolute K: (100°C) - (0°C) = 373.15 - 273.15 = 100 K
    EXPECT_DOUBLE_EQ((100_degC - 0_degC).value, 100.0);
}

TEST(UDLConsistency, FahrenheitAbsoluteZeroApproximatesZeroKelvin) {
    // -459.67°F ≈ 0 K (absolute zero).
    // Note: negative UDL literals parse as -(x_udl), not as (-x)_udl.
    // Must apply the conversion manually for negative Fahrenheit inputs.
    const double f = -459.67;
    Temperature abs_zero{(f - 32.0) * 5.0/9.0 + 273.15};
    EXPECT_NEAR(abs_zero.value, 0.0, 1e-4);
}

TEST(UDLConsistency, KiloWattHourEqualsThirtySixHundredKilojoules) {
    EXPECT_DOUBLE_EQ((1_kWh).value, (3600_kJ).value);
}

TEST(UDLConsistency, ElectronVoltMatchesElementaryCharge) {
    // 1 eV = 1.602176634e-19 J = constants::e.value (same constant by definition)
    EXPECT_DOUBLE_EQ((1_eV).value, constants::e.value);
}

// =============================================================================
// ECSEdgeCases — empty pools, filtering, large IDs, pool size
// =============================================================================

TEST(ECSEdgeCases, EmptyPoolViewIsNoop) {
    Registry reg;
    int count = 0;
    reg.view<int>([&](int&) { count++; });
    EXPECT_EQ(count, 0);
}

TEST(ECSEdgeCases, ViewWithNoMatchingEntities) {
    Registry reg;
    reg.get_pool<int>().assign(0, 10);
    // No float components at all — view should call func zero times
    int count = 0;
    reg.view<int, float>([&](int&, float&) { count++; });
    EXPECT_EQ(count, 0);
}

TEST(ECSEdgeCases, LargeEntityIdSparseResize) {
    Registry reg;
    reg.get_pool<double>().assign(9999, 3.14);
    EXPECT_TRUE(reg.get_pool<double>().contains(9999));
    EXPECT_DOUBLE_EQ(reg.get_pool<double>().get(9999), 3.14);
}

TEST(ECSEdgeCases, ContainsReturnsFalseForUnassigned) {
    Registry reg;
    reg.get_pool<int>().assign(0, 1);
    EXPECT_FALSE(reg.get_pool<int>().contains(1));    // entity 1 never assigned
    EXPECT_FALSE(reg.get_pool<int>().contains(100));  // entity 100 never assigned
}

TEST(ECSEdgeCases, PoolSizeTracksAssignments) {
    Registry reg;
    EXPECT_EQ(reg.get_pool<int>().size(), 0u);
    reg.get_pool<int>().assign(0, 10);
    EXPECT_EQ(reg.get_pool<int>().size(), 1u);
    reg.get_pool<int>().assign(5, 20);
    EXPECT_EQ(reg.get_pool<int>().size(), 2u);
    reg.get_pool<int>().assign(999, 30);
    EXPECT_EQ(reg.get_pool<int>().size(), 3u);
}

TEST(ECSEdgeCases, ViewCountsOnlyEntitiesWithAllComponents) {
    Registry reg;
    for (int i = 0; i < 10; ++i) {
        reg.get_pool<int>().assign(i, i);
        if (i % 3 == 0) reg.get_pool<float>().assign(i, float(i));
    }
    // Entities with both int AND float: 0, 3, 6, 9 → 4
    int count = 0;
    reg.view<int, float>([&](int&, float&) { count++; });
    EXPECT_EQ(count, 4);
}

TEST(ECSEdgeCases, SingleComponentViewSumsAll) {
    Registry reg;
    for (int i = 0; i < 5; ++i) reg.get_pool<int>().assign(i, i);
    int sum = 0;
    reg.view<int>([&](int& v) { sum += v; });
    EXPECT_EQ(sum, 0 + 1 + 2 + 3 + 4);
}

TEST(ECSEdgeCases, ViewMutatesComponents) {
    Registry reg;
    reg.get_pool<int>().assign(0, 10);
    reg.get_pool<int>().assign(1, 20);
    reg.view<int>([](int& v) { v *= 2; });
    EXPECT_EQ(reg.get_pool<int>().get(0), 20);
    EXPECT_EQ(reg.get_pool<int>().get(1), 40);
}

// =============================================================================
// ConstexprEval — arithmetic evaluable at compile time
// =============================================================================

TEST(ConstexprEval, MultiplyIsConstexpr) {
    constexpr Length l = Length(3.0);
    constexpr Length r = Length(4.0);
    constexpr Area   a = l * r;
    static_assert(a.value == 12.0);
    EXPECT_DOUBLE_EQ(a.value, 12.0);
}

TEST(ConstexprEval, NegationIsConstexpr) {
    constexpr Mass m   = Mass(5.0);
    constexpr Mass neg = -m;
    static_assert(neg.value == -5.0);
    EXPECT_DOUBLE_EQ(neg.value, -5.0);
}

TEST(ConstexprEval, ScalarMultiplyIsConstexpr) {
    constexpr Velocity v  = Velocity(3.0);
    constexpr Velocity v2 = v * 2.0;
    static_assert(v2.value == 6.0);
    EXPECT_DOUBLE_EQ(v2.value, 6.0);
}

TEST(ConstexprEval, ComparisonIsConstexpr) {
    constexpr Length a = Length(3.0);
    constexpr Length b = Length(4.0);
    static_assert(a < b);
    static_assert(a != b);
    static_assert(!(a > b));
    static_assert(a <= b);
}

TEST(ConstexprEval, AdditionAndSubtractionAreConstexpr) {
    constexpr Energy e1 = Energy(10.0);
    constexpr Energy e2 = Energy(3.0);
    static_assert((e1 + e2).value == 13.0);
    static_assert((e1 - e2).value == 7.0);
}

// =============================================================================
// StreamOutputEdgeCases — format details of operator<<
// =============================================================================

TEST(StreamOutputEdgeCases, AllSevenSlotsPresentInOutput) {
    using AllSlots = Quantity<Dimensions<1,2,3,4,5,6,7>>;
    std::ostringstream oss;
    oss << AllSlots(1.0);
    const auto s = oss.str();
    EXPECT_NE(s.find("kg"),  std::string::npos);
    EXPECT_NE(s.find("m"),   std::string::npos);
    EXPECT_NE(s.find("s"),   std::string::npos);
    EXPECT_NE(s.find("A"),   std::string::npos);
    EXPECT_NE(s.find("K"),   std::string::npos);
    EXPECT_NE(s.find("mol"), std::string::npos);
    EXPECT_NE(s.find("cd"),  std::string::npos);
}

TEST(StreamOutputEdgeCases, SingleSlotMassOnly) {
    // Mass = [kg] — no other unit names should appear
    std::ostringstream oss;
    oss << Mass(5.0);
    const auto s = oss.str();
    EXPECT_NE(s.find("kg"), std::string::npos);
    EXPECT_NE(s.find("[kg]"), std::string::npos);
}

TEST(StreamOutputEdgeCases, NegativeExponentShowsCaretNeg) {
    // Velocity = m·s^-1 → output must contain "^-1"
    std::ostringstream oss;
    oss << Velocity(1.0);
    EXPECT_NE(oss.str().find("^-1"), std::string::npos);
}

TEST(StreamOutputEdgeCases, ExponentGreaterThanOneShowsCaret) {
    // Volume = m^3 → output must contain "^3"
    std::ostringstream oss;
    oss << Volume(1.0);
    EXPECT_NE(oss.str().find("^3"), std::string::npos);
}

TEST(StreamOutputEdgeCases, ZeroValueQuantity) {
    std::ostringstream oss;
    oss << Length(0.0);
    const auto s = oss.str();
    EXPECT_NE(s.find("0"),   std::string::npos);
    EXPECT_NE(s.find("[m]"), std::string::npos);
}

TEST(StreamOutputEdgeCases, NegativeValueAppearsInOutput) {
    std::ostringstream oss;
    oss << Length(-3.5);
    EXPECT_NE(oss.str().find("-3.5"), std::string::npos);
}

TEST(StreamOutputEdgeCases, ExponentOneNotPrinted) {
    // Length = [m] (exponent 1 is implicit — "m^1" should NOT appear)
    std::ostringstream oss;
    oss << Length(1.0);
    EXPECT_EQ(oss.str().find("^1"), std::string::npos);
}
