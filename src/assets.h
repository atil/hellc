#pragma once
#include <string>
#include <vector>

class ObjFaceData {
public:
    std::string material_name;
    std::vector<int> indices;

    ObjFaceData() { }

    ObjFaceData(const ObjFaceData& other) {
        this->material_name = other.material_name;
        this->indices = other.indices;
    }
};

class ObjModelData {
public:
    std::string mtllib_path;

    // TODO: These aren't mutable stuff. Consider something else than a vector
    std::vector<float> position_data;
    std::vector<float> uv_data;
    std::vector<float> normal_data;
    std::vector<ObjFaceData> face_data;

    ObjModelData(const std::string& file_path);
};
