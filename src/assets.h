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

struct Material {
    std::string name;
    std::string diffuse_texture_name;
    Vector3 diffuse;
    float transparency { 0 };
};

struct ObjModelData {
    std::vector<Material> materials;

    std::vector<Vector3> position_data;
    std::vector<Vector2> uv_data;
    std::vector<Vector3> normal_data;

    std::vector<ObjSubmodelData> submodel_data;

    explicit ObjModelData(const std::string& file_path);
};

struct WorldspawnEntry {
    std::string obj_name;
    Vector3 position;
    Vector3 rotation;
};

struct PropEntry {
    std::string obj_name;
    Vector3 position;
    Vector3 rotation;
};

struct PointLightInfo {
    Vector3 position;
    Vector3 color;
    float intensity;
    float attenuation;
};

struct DirectionalLightInfo {
    Vector3 position;
    Vector3 color;
};

struct Scene {
    Vector3 player_start;
    Vector3 player_lookat;
    std::vector<WorldspawnEntry> worldspawn;
    std::vector<PropEntry> props;
    std::vector<PointLightInfo> point_light_info;
    DirectionalLightInfo directional_light_info;
};

Scene read_scene(const std::string& file_path);
