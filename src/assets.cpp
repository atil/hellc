#include "assets.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define OBJ_NAME_MAX_LENGTH 50

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

ObjFileData* load_obj(const char* file_path, float** vertex_positions, int* vertex_count) {
    FILE* stream = fopen(file_path, "rb");
    if (!stream) {
        printf("Failed to open obj file %s", file_path);
        return NULL;
    }

    //
    // Figure out how many position/uv/normals are in the file
    //

    int position_count = 0;
    int face_count = 0;
    int uv_count = 0;
    int normal_count = 0;
    while (true) {
        char line[128];

        if (fscanf(stream, "%s", line) == EOF) {
            break;
        }
        if (strcmp(line, "#") == 0) {
            continue;
        } else if (strcmp(line, "v") == 0) {
            position_count++;
        } else if (strcmp(line, "f") == 0) {
            face_count++;
        } else if (strcmp(line, "vt") == 0) {
            uv_count++;
        } else if (strcmp(line, "vn") == 0) {
            normal_count++;
        }
    }

    // TODO @PERF: Maybe doing this in a big string is better
    // File seek operation is expensive
    rewind(stream); 

    ObjFileData* obj_data = (ObjFileData*) malloc(sizeof(ObjFileData));
    obj_data->name = (char*) malloc(OBJ_NAME_MAX_LENGTH * sizeof(char));

    float* positions = (float*) malloc(position_count * 3 * sizeof(float));
    float* uvs = (float*) malloc(uv_count * 2 * sizeof(float));
    float* normals = (float*) malloc(uv_count * 3 * sizeof(float));
    int* face_data = (int*) malloc(face_count * 9 * sizeof(int));

    int position_index = 0;
    int uv_index = 0;
    int normal_index = 0;
    int face_index = 0;

    //
    // Read the vertex/uv/normal/face data into memory
    //

    while (true) {
        char line[128];

        if (fscanf(stream, "%s", line) == EOF) {
            break;
        }

        if (strcmp(line, "#") == 0) {
            continue;
        } else if (strcmp(line, "o") == 0) {
            fscanf(stream, "%s", obj_data->name);
        } else if (strcmp(line, "v") == 0) {
            float x, y, z;
            fscanf(stream, "%f %f %f", &x, &y, &z);
            positions[position_index++] = x;
            positions[position_index++] = y;
            positions[position_index++] = z;
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
        } else if (strcmp(line, "f") == 0) {
            int pos_x_index, pos_y_index, pos_z_index;
            int uv_x_index, uv_y_index, uv_z_index;
            int norm_x_index, norm_y_index, norm_z_index;
            fscanf(stream, "%d/%d/%d %d/%d/%d %d/%d/%d", 
                    &pos_x_index, &uv_x_index, &norm_x_index,
                    &pos_y_index, &uv_y_index, &norm_y_index,
                    &pos_z_index, &uv_z_index, &norm_z_index);

            face_data[face_index++] = pos_x_index - 1;
            face_data[face_index++] = uv_x_index - 1;
            face_data[face_index++] = norm_x_index - 1;

            face_data[face_index++] = pos_y_index - 1;
            face_data[face_index++] = uv_y_index - 1;
            face_data[face_index++] = norm_y_index - 1;

            face_data[face_index++] = pos_z_index - 1;
            face_data[face_index++] = uv_z_index - 1;
            face_data[face_index++] = norm_z_index - 1;
        }
    }

    // 
    // Set batched data to be given directly to opengl
    //

    const int vertex_length = 3 + 2 + 3; // pos - uv - normal
    obj_data->batched_data_length = face_count * (3 * vertex_length); // 3 vertices per face
    obj_data->batched_data = (float*) malloc(obj_data->batched_data_length * sizeof(float));

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

    obj_data->batched_index_length = face_count * 3; // 3 vertices per face
    obj_data->batched_index_data = (int*) malloc(obj_data->batched_index_length * sizeof(int));

    for (int i = 0, index_data_index = 0; i < face_count; i++) {
        obj_data->batched_index_data[index_data_index++] = i * 3;
        obj_data->batched_index_data[index_data_index++] = i * 3 + 1;
        obj_data->batched_index_data[index_data_index++] = i * 3 + 2;
    }

    fclose(stream);
    free(face_data);
    free(uvs);
    free(normals);

    *vertex_positions = positions;
    *vertex_count = position_count;
    return obj_data;
}

unsigned char* read_image(const char* image_path, int* out_width, int* out_height) {
    stbi_set_flip_vertically_on_load(true);
    int n;
    unsigned char *data = stbi_load(image_path, out_width, out_height, &n, 0);
    if (data == nullptr) {
        printf("Couldn't load the image at %s\n", image_path);
        return nullptr;
    }

    return data;
}

void free_image(unsigned char* image_data) {
    stbi_image_free(image_data);
}

void free_obj(ObjFileData* obj) {
    free(obj->name);
    free(obj->batched_data);
    free(obj->batched_index_data);
}
