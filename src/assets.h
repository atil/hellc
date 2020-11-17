#ifndef _ASSETS_H_
#define _ASSETS_H_

#include <vector>

struct ObjFileData {
    char* name;

    float* batched_data;
    int batched_data_length;

    int* batched_index_data;
    int batched_index_length;
};

struct Material {
    char* name;
    char* diffuse_texture_name;
    float* diffuse;
    float transparency;
};

ObjFileData* load_obj(const char* file_path);
void free_obj(ObjFileData* obj);

char* read_file(const char* file_path);

unsigned char* read_image(const char* image_path, int* out_width, int* out_height);
void free_image(unsigned char* image_data);

std::vector<Material> load_mtl_file(const char* file_path);
#endif
