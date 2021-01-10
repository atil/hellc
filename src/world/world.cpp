#include "world.h"

StaticCollider::StaticCollider(const ObjModelData& obj_data, const Vector3& position, const Vector3& rotation) {
    const std::vector<Vector3>& position_data = obj_data.position_data;

    for (const ObjSubmodelData& obj_submodel_data : obj_data.submodel_data) {
        for (const ObjFaceData& face_data : obj_submodel_data.faces) {
            const size_t* position_indices = face_data.position_indices;
            const Vector3& p0 = Vector3::rotate(position_data[position_indices[0]], rotation) + position;
            const Vector3& p1 = Vector3::rotate(position_data[position_indices[1]], rotation) + position;
            const Vector3& p2 = Vector3::rotate(position_data[position_indices[2]], rotation) + position;

            this->triangles.emplace_back(p0, p1, p2);
        }
    }
}

void World::register_scene(const Scene& scene) {
    this->player_position = scene.player_start;
    this->player_forward = Vector3::normalize(scene.player_lookat - scene.player_start);

    for (const WorldspawnEntry& entry : scene.worldspawn) {
        const ObjModelData obj_data(entry.obj_name);
        register_static_collider(obj_data, entry.position, entry.rotation);
    }
}

void World::register_static_collider(const ObjModelData& obj_data, const Vector3& position, const Vector3& rotation) {
    this->static_colliders.emplace_back(obj_data, position, rotation);
}
