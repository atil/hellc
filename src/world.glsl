#ifdef VERTEX
layout (location = 0) in vec3 in_position;
layout (location = 1) in vec2 in_uv;
layout (location = 2) in vec3 in_normal;

uniform mat4 u_model;
uniform mat4 u_view;
uniform mat4 u_perspective;

out vec2 v2f_uv;
out vec3 v2f_normal;

void main() {
    v2f_uv = in_uv;
    v2f_normal = in_normal;

    gl_Position = u_perspective * u_view * u_model * vec4(in_position, 1.0);
};

#endif

#ifdef FRAGMENT
in vec2 v2f_uv;
in vec3 v2f_normal;

out vec4 frag_color;

void main() {
    float tmp = v2f_uv.x * v2f_normal.x * 0.00001f;

    float uv_max = max(v2f_uv.x, v2f_uv.y);
    vec2 uv_normalized = v2f_uv / uv_max;

    vec3 color = vec3(uv_normalized, 0);
    frag_color = vec4(color, 1.0);
};
#endif
