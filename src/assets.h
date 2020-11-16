#ifndef _ASSETS_H_
#define _ASSETS_H_

struct ObjFileData {
    char* name;

    float* batched_data;
    int batched_data_length;

    int* batched_index_data;
    int batched_index_length;
};

struct MtlFileData {

};

ObjFileData* load_obj(const char* file_path, float** vertex_positions, int* vertex_count);
void free_obj(ObjFileData* obj);

char* read_file(const char* file_path);

unsigned char* read_image(const char* image_path, int* out_width, int* out_height);
void free_image(unsigned char* image_data);

#endif
