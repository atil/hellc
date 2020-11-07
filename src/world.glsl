#ifdef VERTEX
in vec3 vp;
void main() {
    gl_Position = vec4(vp, 1.0);
};

#endif

#ifdef FRAGMENT
out vec4 frag_color;

void main() {
    frag_color = vec4(1.0, 0.0, 0.5, 1.0);
};
#endif
