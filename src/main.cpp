// TODO
// - problem with reading test_lighting.obj
// - take a look if there's any c-like thing is remaining

#include "render/render.h"
#include "assets.h"
#include "world.h"
#include "platform.h"

int main() {

    Platform platform; // Probably should be the first thing
    Renderer renderer;
    const ObjModelData obj_data("assets/test_lighting.obj");
    renderer.register_obj(obj_data);

    Player player;

    float prev_time = static_cast<float>(Platform::get_time());
    while (!platform.should_window_close()) {
        const float now_time = static_cast<float>(Platform::get_time());
        const float delta_time = now_time - prev_time;
        prev_time = now_time;

        platform.fill_input();
        player.process_input(platform.get_input(), delta_time);

        renderer.render(player.get_view_matrix());

        platform.end_frame();
    }

    return 0;
}
