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
