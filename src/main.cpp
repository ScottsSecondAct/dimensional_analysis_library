#include "dimensions.h"
#include "ecs.h"
#include <iostream>

struct Position { double x; };
struct VelocityComp { double dx; };

int main() {
    Registry reg;
    
    std::cout << "--- Phase 1: Dimensional Analysis ---" << std::endl;
    auto dist = 10.0_m;
    auto time = 2.0_s;
    auto speed = dist.value / time.value;
    std::cout << "Calculated Speed: " << speed << " m/s" << std::endl;

    std::cout << "\n--- Phase 2: ECS Sparse Set ---" << std::endl;
    for(int i = 0; i < 5; ++i) {
        reg.get_pool<Position>().assign(i, { (double)i });
        if (i % 2 == 0) reg.get_pool<VelocityComp>().assign(i, { 1.5 });
    }

    reg.view<Position, VelocityComp>([](auto& pos, auto& vel) {
        pos.x += vel.dx;
        std::cout << "Entity Updated. New Pos: " << pos.x << std::endl;
    });

    return 0;
}