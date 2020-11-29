#include "world.h"

StaticCollider::StaticCollider(const ObjModelData& obj_data) {
    const std::vector<float>& position_data = obj_data.position_data;

    for (const ObjFaceData& obj_face : obj_data.face_data) {
        const std::vector<int>& indices = obj_face.indices;
        for (size_t i = 0; i < indices.size(); i += 9) {
            const float x0 = position_data[indices[i] * 3];
            const float y0 = position_data[indices[i] * 3 + 1];
            const float z0 = position_data[indices[i] * 3 + 2];
            glm::vec3 p0(x0, y0, z0);

            const float x1 = position_data[indices[i + 3] * 3];
            const float y1 = position_data[indices[i + 3] * 3 + 1];
            const float z1 = position_data[indices[i + 3] * 3 + 2];
            glm::vec3 p1(x1, y1, z1);

            const float x2 = position_data[indices[i + 6] * 3];
            const float y2 = position_data[indices[i + 6] * 3 + 1];
            const float z2 = position_data[indices[i + 6] * 3 + 2];
            glm::vec3 p2(x2, y2, z2);

            this->triangles.emplace_back(p0, p1, p2);
        }
    }
}

const std::vector<Triangle>& StaticCollider::get_triangles() const {
    return this->triangles;
}

