#include "units.h"
#include "ecs.h"
#include <iostream>

struct Position { double x; };
struct VelocityComp { double dx; };

int main() {
    std::cout << "--- Phase 1: Mechanics ---" << std::endl;
    auto dist     = 10.0_m;
    auto duration = 2.0_s;
    auto speed    = dist / duration;   // yields Velocity, not double
    std::cout << "Speed: " << speed.value << " m/s\n";

    auto mass   = 5.0_kg;
    auto accel  = 9.81_m / 1.0_s / 1.0_s;
    auto weight = mass * accel;        // yields Force
    std::cout << "Weight: " << weight.value << " N\n";

    std::cout << "\n--- Phase 2: Chemistry & Thermodynamics ---" << std::endl;
    auto heat     = 1.0_kcal;          // Energy stored in joules (4184.0 J)
    auto bodyTemp = 37.0_degC;         // Temperature stored in kelvin (310.15 K)
    auto moles    = 0.5_mol;
    std::cout << "Heat: "      << heat.value     << " J\n";
    std::cout << "Body temp: " << bodyTemp.value << " K\n";
    std::cout << "Moles: "     << moles.value    << " mol\n";

    auto pressure = 1.0_atm;           // Pressure stored in Pa (101325 Pa)
    std::cout << "Pressure: "  << pressure.value << " Pa\n";

    std::cout << "\n--- Phase 3: ECS Sparse Set ---" << std::endl;
    Registry reg;
    for (int i = 0; i < 5; ++i) {
        reg.get_pool<Position>().assign(i, { (double)i });
        if (i % 2 == 0) reg.get_pool<VelocityComp>().assign(i, { 1.5 });
    }

    reg.view<Position, VelocityComp>([](auto& pos, auto& vel) {
        pos.x += vel.dx;
        std::cout << "Entity Updated. New Pos: " << pos.x << "\n";
    });

    return 0;
}
