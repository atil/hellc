// TODO @NEXT: Read objects/properties from file
//  - Register the directional light to renderer
// TODO @NEXT: Wiggly torch-lights
// TODO @NEXT: Fix light bleed and peter-panning

#include "render/render.h"
#include "assets.h"
#include "platform.h"
#include "world/world.h"

// TODO @TASK @DEV: Separate unit test project
// TODO @TASK @DEV: Test release build
//#define RUN_TESTS 

#ifdef RUN_TESTS
int main() {
    Physics::run_geom_tests();
    Physics::run_collision_tests();
    getchar();
    return 0;
}
#else

int main() {
    Platform platform; // Probably should be the first thing
    Renderer renderer;
    World world;

    Scene scene = read_scene("assets/test_lighting.txt");
    for (const WorldspawnEntry& entry : scene.worldspawn) {
        const ObjModelData obj_data(entry.obj_name);
        renderer.register_static_obj(obj_data, entry.position, entry.rotation);
        world.register_static_collider(obj_data, entry.position, entry.rotation);
    }

    for (const PointLightInfo& point_light_info : scene.point_light_info) {
        renderer.register_point_light(point_light_info);
    }

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
