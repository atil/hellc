#include "world.h"

// TODO @CLEANUP: Could move this to physics.cpp
StaticCollider::StaticCollider(const ObjModelData& obj_data) {
    const std::vector<Vector3>& position_data = obj_data.position_data;

    for (const ObjSubmodelData& obj_submodel_data : obj_data.submodel_data) {
        for (const ObjFaceData& face_data : obj_submodel_data.faces) {
            const size_t* position_indices = face_data.position_indices;
            const Vector3& p0 = position_data[position_indices[0]];
            const Vector3& p1 = position_data[position_indices[1]];
            const Vector3& p2 = position_data[position_indices[2]];

            this->triangles.emplace_back(p0, p1, p2);
        }
    }
}

