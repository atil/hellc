// TODO @NEXT: Read objects/properties from file
//  - A txt file containing (obj name - position - rotation) triples
// TODO @NEXT: Wiggly torch-lights

#include "render/render.h"
#include "assets.h"
#include "platform.h"
#include "world/world.h"

//#define RUN_TESTS

#ifdef RUN_TESTS
int main() {
    const ObjModelData obj_data("assets/test_lighting.obj");
    World world;
    world.register_static_collider(obj_data, Vector3::zero, Vector3::zero);
    Physics::run_geom_tests();
    Physics::run_collision_tests();
    getchar();
    return 0;
}

#else

int main() {

    Platform platform; // Probably should be the first thing
    Renderer renderer;

    Vector3 pos_world(0, 0, 0);
    Vector3 rot_world(0, 0, 0);
    const ObjModelData obj_data_world("assets/test_lighting.obj");
    renderer.register_static_obj(obj_data_world, pos_world, rot_world);

    Vector3 pos_triangle(-3, 1, 5);
    Vector3 rot_triangle(0, 0, 0);
    const ObjModelData obj_data_triangle("assets/triangle.obj");
    renderer.register_static_obj(obj_data_triangle, pos_triangle, rot_triangle);

    World world;
    world.register_static_collider(obj_data_world, pos_world, rot_world);

    float prev_time = Platform::get_time();
    while (!platform.should_window_close()) {
        const float now_time = Platform::get_time();
        const float dt = now_time - prev_time;
        prev_time = now_time;

        platform.read_input();
        world.tick(platform, dt);
        renderer.render(world.get_view_matrix());

        platform.end_frame();
    }

    return 0;
}
#endif
