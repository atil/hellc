#ifdef VERTEX
layout (location = 0) in vec3 in_position;

uniform mat4 u_mvp;

void main() {
    gl_Position = u_mvp * vec4(in_position, 1.0);
};

#endif

#ifdef FRAGMENT
out vec4 frag_color;

void main() {
    frag_color = vec4(1.0, 0.0, 0.5, 1.0);
};
#endif
