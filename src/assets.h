#ifndef _ASSETS_H_
#define _ASSETS_H_

struct ObjFileData {
    char* name;

    float* batched_data;
    int batched_data_length;

    int* batched_index_data;
    int batched_index_length;
};

ObjFileData* load_obj(const char* file_path, float** vertex_positions, int* vertex_count);

void delete_obj(ObjFileData* obj);

char* read_file(const char* file_path);

#endif
