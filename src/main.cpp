// TODO gl errors: useProgram returns error (programids are only 3 and 6. is this normal?)
// TODO implement one shot key press (prevkeys stuff)
// TODO vim dd is sometimes problematic, find out when
// TODO BACKLOG Vertex colors

#include "render/render.h"
#include "assets.h"
#include "platform.h"
#include "world/world.h"

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
