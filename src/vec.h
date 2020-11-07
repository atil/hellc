typedef struct {
    float x;
    float y;
    float z;
} Vec3;

Vec3 VecAdd(Vec3 a, Vec3 b) {
    Vec3 r = {a.x + b.x, a.y + b.y, a.z + b.z};
    return r;
}

typedef struct {
    int x;
    int y;
    int z;
} Vec3i;
