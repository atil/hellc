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

ObjFileData* load_obj(const char* file_path) {
    FILE* stream = fopen(file_path, "rb");
    if (!stream) {
        printf("Failed to open obj file %s", file_path);
        return NULL;
    }

    int position_count = 0;
    int face_count = 0;
    int uv_count = 0;
    int normal_count = 0;
    while (true) {
        char line[128];

        if (fscanf(stream, "%s", line) == EOF) {
            break;
        }

        if (strcmp(line, "v") == 0) {
            position_count++;
        } else if (strcmp(line, "f") == 0) {
            face_count++;
        } else if (strcmp(line, "vt") == 0) {
            uv_count++;
        } else if (strcmp(line, "vn") == 0) {
            normal_count++;
        }
    }

    // TODO @PERF: Maybe doing this in a string is better
    // File seek operation is expensive
    rewind(stream); 

    ObjFileData* obj_data = (ObjFileData*) malloc(sizeof(ObjFileData));
    float* positions = (float*) malloc(position_count * 3 * sizeof(float));
    int* face_data = (int*) malloc(face_count * 9 * sizeof(int));
    float* uvs = (float*) malloc(uv_count * 2 * sizeof(float));
    float* normals = (float*) malloc(uv_count * 3 * sizeof(float));

    int position_index = 0;
    int face_index = 0;
    int uv_index = 0;
    int normal_index = 0;

    while (true) {
        char line[128];

        if (fscanf(stream, "%s", line) == EOF) {
            break;
        }

        if (strcmp(line, "v") == 0) {
            float x, y, z;
            fscanf(stream, "%f %f %f", &x, &y, &z);
            positions[position_index++] = x;
            positions[position_index++] = y;
            positions[position_index++] = z;
        } else if (strcmp(line, "f") == 0) {
            int pos_x_index, pos_y_index, pos_z_index;
            int uv_x_index, uv_y_index, uv_z_index;
            int norm_x_index, norm_y_index, norm_z_index;
            fscanf(stream, "%d/%d/%d %d/%d/%d %d/%d/%d", 
                    &pos_x_index, &uv_x_index, &norm_x_index,
                    &pos_y_index, &uv_y_index, &norm_y_index,
                    &pos_z_index, &uv_z_index, &norm_z_index);
            face_data[face_index++] = pos_x_index;
            face_data[face_index++] = uv_x_index;
            face_data[face_index++] = norm_x_index;

            face_data[face_index++] = pos_y_index;
            face_data[face_index++] = uv_y_index;
            face_data[face_index++] = norm_y_index;

            face_data[face_index++] = pos_z_index;
            face_data[face_index++] = uv_z_index;
            face_data[face_index++] = norm_z_index;
        } else if (strcmp(line, "vt") == 0) {
            float u, v;
            fscanf(stream, "%f %f", &u, &v);
            uvs[uv_index++] = u;
            uvs[uv_index++] = v;
        } else if (strcmp(line, "vn") == 0) {
            float nx, ny, nz;
            fscanf(stream, "%f %f %f", &nx, &ny, &nz);
            normals[normal_index++] = nx;
            normals[normal_index++] = ny;
            normals[normal_index++] = nz;
        }
    }

    // 
    // Set batched data to be given directly to opengl
    //

    int vertex_size = 3 * sizeof(float) + 2 * sizeof(float) + 3 * sizeof(float);
    obj_data->batched_data_size = face_count * (3 * vertex_size);
    obj_data->batched_data = (float*) malloc(sizeof(float) * obj_data->batched_data_size);

    bool has_uv = uv_count > 0;
    bool has_norm = normal_count > 0;

    for (int i = 0, batched_data_index = 0; i < face_count; i++) {
        int face_start_index = i * 9;
        int pos_x_index = face_data[face_start_index];
        int uv_x_index = face_data[face_start_index + 1];
        int norm_x_index = face_data[face_start_index + 2];

        int pos_y_index = face_data[face_start_index + 3];
        int uv_y_index = face_data[face_start_index + 4];
        int norm_y_index = face_data[face_start_index + 5];

        int pos_z_index = face_data[face_start_index + 6];
        int uv_z_index = face_data[face_start_index + 7];
        int norm_z_index = face_data[face_start_index + 8];

        // TODO @PERF: Group the array usages together for cache-friendliness
        // That'd require each float's index to be precalculated in the batched array

        // First vertex's data
        obj_data->batched_data[batched_data_index++] = positions[3 * pos_x_index];
        obj_data->batched_data[batched_data_index++] = positions[3 * pos_x_index + 1];
        obj_data->batched_data[batched_data_index++] = positions[3 * pos_x_index + 2];
        obj_data->batched_data[batched_data_index++] = uvs[2 * uv_x_index];
        obj_data->batched_data[batched_data_index++] = uvs[2 * uv_x_index + 1];
        obj_data->batched_data[batched_data_index++] = normals[3 * norm_x_index];
        obj_data->batched_data[batched_data_index++] = normals[3 * norm_x_index + 1];
        obj_data->batched_data[batched_data_index++] = normals[3 * norm_x_index + 2];

        // Second vertex's data
        obj_data->batched_data[batched_data_index++] = positions[3 * pos_y_index];
        obj_data->batched_data[batched_data_index++] = positions[3 * pos_y_index + 1];
        obj_data->batched_data[batched_data_index++] = positions[3 * pos_y_index + 2];
        obj_data->batched_data[batched_data_index++] = uvs[2 * uv_y_index];
        obj_data->batched_data[batched_data_index++] = uvs[2 * uv_y_index + 1];
        obj_data->batched_data[batched_data_index++] = normals[3 * norm_y_index];
        obj_data->batched_data[batched_data_index++] = normals[3 * norm_y_index + 1];
        obj_data->batched_data[batched_data_index++] = normals[3 * norm_y_index + 2];

        // Third vertex's data
        obj_data->batched_data[batched_data_index++] = positions[3 * pos_z_index];
        obj_data->batched_data[batched_data_index++] = positions[3 * pos_z_index + 1];
        obj_data->batched_data[batched_data_index++] = positions[3 * pos_z_index + 2];
        obj_data->batched_data[batched_data_index++] = uvs[2 * uv_z_index];
        obj_data->batched_data[batched_data_index++] = uvs[2 * uv_z_index + 1];
        obj_data->batched_data[batched_data_index++] = normals[3 * norm_z_index];
        obj_data->batched_data[batched_data_index++] = normals[3 * norm_z_index + 1];
        obj_data->batched_data[batched_data_index++] = normals[3 * norm_z_index + 2];
    }

    obj_data->batched_index_count = face_count * 3;
    obj_data->batched_index_data = (int*) malloc(sizeof(int) * 3 * face_count);

    for (int i = 0, index_data_index = 0; i < face_count; i++) {
        int vertex_index_0 = face_data[(i * 9)];
        int vertex_index_1 = face_data[(i * 9) + 3];
        int vertex_index_2 = face_data[(i * 9) + 6];
        obj_data->batched_index_data[index_data_index++] = vertex_index_0;
        obj_data->batched_index_data[index_data_index++] = vertex_index_1;
        obj_data->batched_index_data[index_data_index++] = vertex_index_2;
    }

    fclose(stream);
    free(positions);
    free(face_data);
    free(uvs);
    free(normals);
    return obj_data;
}


void delete_obj(ObjFileData* obj) {
    free(obj->name);
    free(obj->batched_data);
    free(obj->batched_index_data);
}
