#ifndef _ASSETS_H_
#define _ASSETS_H_

#include <vector>
#include <string>

class ObjFileData {
public:
    char* name;

    float* batched_data;
    int batched_data_length;

    int* batched_index_data;
    int batched_index_length;

    ObjFileData(const std::string& file_path);
    ~ObjFileData();
};

class Image {
public:
    int width;
    int height;
    unsigned char* image_data;

    Image(const std::string& file_path);
    ~Image();
};

struct Material {
    char* name;
    char* diffuse_texture_name;
    float* diffuse;
    float transparency;
};

char* read_file(const char* file_path);

std::vector<Material> load_mtl_file(const char* file_path);
#endif
