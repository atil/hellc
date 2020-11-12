#ifndef _ASSETS_H_
#define _ASSETS_H_

struct ObjFileData {
    char* name;

    float* batched_data;
    int batched_data_size;

    int* batched_index_data;
    int batched_index_count;
};

ObjFileData* load_obj(const char* file_path);

void delete_obj(ObjFileData* obj);

char* read_file(const char* file_path);

#endif
