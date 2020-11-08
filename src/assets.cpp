#include "assets.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

char* read_file(const char* file_path) {
    FILE* f = fopen(file_path, "rb");
    if (!f) {
        printf("failed to open file %s", file_path);
        return NULL;
    }

    fseek(f, 0, SEEK_END);
    long length = ftell(f);
    fseek(f, 0, SEEK_SET);

    char* buffer = (char*) malloc(length + 1);
    fread(buffer, sizeof(char), length, f);
    fclose(f);
    buffer[length] = 0;

    return buffer;
}

void load_obj(const char* file_path, char** object_name, float** vertices, int* vertex_count, int** faces, int* face_count) {
    char* obj_str = read_file(file_path);

    char* obj_str_copy = (char*) malloc(strlen(obj_str));
    strcpy(obj_str_copy, obj_str);

    const char* newline = "\r\n";

    *vertex_count = 0;
    *face_count = 0;

    // Determine lengths
    for (char* line = strtok(obj_str_copy, newline);
            line != NULL;
            line = strtok(NULL, newline)) {
        switch (*(line)) {
            case 'v':
                (*vertex_count)++;
                break;
            case 'f':
                (*face_count)++;
                break;
        }
    }

    //
    // Parse information
    //

    *vertices = (float*) malloc((*vertex_count) * 3 * sizeof(float));
    *faces = (int*) malloc((*face_count) * 3 * sizeof(int));

    int i_vertex = 0;
    int i_face = 0;

    for (char* line = strtok(obj_str, newline);
            line != NULL;
            line = strtok(NULL, newline)) {
        char tmp;
        int len;
        switch (*(line)) {
            case 'o':
                len = strlen(line);
                *object_name = (char*) malloc(len);
                sscanf(line, "%c %s", &tmp, *object_name);
                break;
            case 'v':
                float x, y, z;
                sscanf(line,"%c %f %f %f", &tmp, &x, &y, &z);
                (*vertices)[i_vertex++] = x;
                (*vertices)[i_vertex++] = y;
                (*vertices)[i_vertex++] = z;
                break;
            case 'f':
                int i1, i2, i3;
                sscanf(line,"%c %d %d %d", &tmp, &i1, &i2, &i3);
                (*faces)[i_face++] = i1;
                (*faces)[i_face++] = i2;
                (*faces)[i_face++] = i3;
                break;
        }
    }

    free(obj_str);
}
