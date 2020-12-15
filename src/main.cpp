// TODO @NEXT: Call the directional light render function
// TODO @NEXT: Edit shader with directional light

#include "render/render.h"
#include "assets.h"
#include "platform.h"
#include "world/world.h"

//#define RUN_TESTS

#ifdef RUN_TESTS
int main() {
    const ObjModelData obj_data("assets/test_lighting.obj");
    World world;
    world.register_static_collider(obj_data);
    Physics::run_geom_tests();
    Physics::run_collision_tests();
    getchar();
    return 0;
}

#else

int main() {
    Platform platform; // Probably should be the first thing
    Renderer renderer;
    const ObjModelData obj_data("assets/test_lighting.obj");
    renderer.register_obj(obj_data);

    World world;
    world.register_static_collider(obj_data);

    float prev_time = static_cast<float>(Platform::get_time());
    while (!platform.should_window_close()) {
        const float now_time = static_cast<float>(Platform::get_time());
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
