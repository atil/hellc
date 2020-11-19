#ifndef _ASSETS_H_
#define _ASSETS_H_

#include <vector>
#include <string>

struct Material {
    std::string name;
    std::string diffuse_texture_name;
    float diffuse[3];
    float transparency;
};

class RenderUnit {
public:
    Material material;
    float* vertex_data;
    int vertex_data_length;

    int* index_data;
    int index_data_length;

    RenderUnit(Material material, const std::vector<int>& face_data, const std::vector<float>& position_data, const std::vector<float>& uv_data, const std::vector<float>& normal_data);
    ~RenderUnit();
};

std::vector<RenderUnit*> load_obj_file(const std::string& file_path);

class Image {
public:
    int width;
    int height;
    unsigned char* image_data;

    Image(const std::string& file_path);
    ~Image();
};

char* read_file(const char* file_path);

#endif
