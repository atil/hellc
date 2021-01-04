#include "world.h"

void World::register_scene(const Scene& scene) {
    
    this->player_position = scene.player_start;

    for (const WorldspawnEntry& entry : scene.worldspawn) {
        const ObjModelData obj_data(entry.obj_name);
        register_static_collider(obj_data, entry.position, entry.rotation);
    }
}
