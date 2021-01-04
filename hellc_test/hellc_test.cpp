#include <iostream>
#include "../src/vector3.h"
#include "../src/world/world.h"
#include "../src/util.h"
#include "tests/tests.h"

int main()
{
    run_geom_tests();
    run_collision_tests();
    getchar();
    return 0;
}
