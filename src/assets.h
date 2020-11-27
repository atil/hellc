#pragma once
#include <string>
#include <vector>

struct ObjFaceData {
    std::string material_name;
    std::vector<int> indices;

    ObjFaceData() = default;

    ObjFaceData(const ObjFaceData& other) {
        this->material_name = other.material_name;
        this->indices = other.indices;
    }
};

struct ObjModelData {
    std::string mtllib_path;

    // TODO @CLEANUP: These aren't mutable stuff. Consider using const vectors
    // TODO @CLEANUP: Keep vec3's instead of floats. This is tricky to index
    std::vector<float> position_data;
    std::vector<float> uv_data;
    std::vector<float> normal_data;
    std::vector<ObjFaceData> face_data;

    explicit ObjModelData(const std::string& file_path);
};
