#ifdef VERTEX
layout (location = 0) in vec3 in_position;

uniform mat4 u_model;
uniform mat4 u_view;
uniform mat4 u_perspective;

void main() {
    gl_Position = u_perspective * u_view * u_model * vec4(in_position, 1.0);
};

#endif

#ifdef FRAGMENT
out vec4 frag_color;

void main() {
    frag_color = vec4(1.0, 0.0, 0.5, 1.0);
};
#endif
