// TODO @NEXT: Player start pos
// TODO @NEXT: Shoot footage

#include "render/render.h"
#include "assets.h"
#include "platform.h"
#include "world/world.h"

#include <direct.h>

int main() {

    // TODO @CLEANUP: To make it run from Unity
    _chdir(R"(c:\users\atil\code\hellc\)");

    Platform platform {};
    Renderer renderer {};
    World world {};

    Scene scene = read_scene("assets/test_export.txt");

    world.register_scene(scene);
    renderer.register_scene(scene);

    float prev_time = Platform::get_time();
    while (!platform.should_window_close()) {
        const float now_time = Platform::get_time();
        const float dt = now_time - prev_time;
        prev_time = now_time;

        platform.read_input();
        world.player_tick(platform, dt);
        renderer.render(world.get_view_matrix(), dt);

        platform.end_frame();
    }

    return 0;
}
