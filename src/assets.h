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

class ObjFileData {
private:
    std::vector<Material> load_mtl_file(const std::string& file_path);

public:
    std::string name;

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


char* read_file(const char* file_path);

#endif
