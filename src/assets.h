#pragma once
#include <string>
#include <vector>

#include "vector3.h"

struct ObjFaceData {
    size_t position_indices[3];
    size_t uv_indices[3];
    size_t normal_indices[3];
};

struct ObjSubmodelData {
    std::string material_name;
    std::vector<ObjFaceData> faces;

    ObjSubmodelData() = default;

    ObjSubmodelData(const ObjSubmodelData& other) {
        this->material_name = other.material_name;
        this->faces = other.faces;
    }
};

struct ObjModelData {
    std::string mtllib_path;

    std::vector<Vector3> position_data;
    std::vector<Vector2> uv_data;
    std::vector<Vector3> normal_data;

    std::vector<ObjSubmodelData> submodel_data;

    explicit ObjModelData(const std::string& file_path);
};
