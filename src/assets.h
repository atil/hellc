#ifndef _ASSETS_H_
#define _ASSETS_H_

void load_obj(const char* file_path, char** object_name, float** vertices, int* vertex_count, int** faces, int* face_count);

char* read_file(const char* file_path);

#endif
