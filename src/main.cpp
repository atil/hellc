// TODO @TASK implement actual collision resolving
// TODO @TASK implement one shot key press (prevkeys stuff)
// TODO vim dd is sometimes problematic, find out when
// TODO @BACKLOG Vertex colors
// TODO @BACKLOG Own vec3 class

#include "render/render.h"
#include "assets.h"
#include "platform.h"
#include "world/world.h"

//#define RUN_TESTS

#ifdef RUN_TESTS
int main() {
    const ObjModelData obj_data("assets/test_lighting.obj");
    Physics physics;
    physics.register_obj(obj_data);
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

    Player player;
    Physics physics;
    physics.register_obj(obj_data);

    float prev_time = static_cast<float>(Platform::get_time());
    while (!platform.should_window_close()) {
        const float now_time = static_cast<float>(Platform::get_time());
        const float dt = now_time - prev_time;
        prev_time = now_time;

        platform.read_input();
        player.process_input(platform.get_input(), dt);
        physics.tick(player.position, dt);

        renderer.render(player.get_view_matrix());

        platform.end_frame();
    }

    return 0;
}
#endif
