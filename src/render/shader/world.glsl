#ifdef VERTEX
layout (location = 0) in vec3 in_position;
layout (location = 1) in vec2 in_uv;
layout (location = 2) in vec3 in_normal;

uniform mat4 u_model;
uniform mat4 u_view;
uniform mat4 u_projection;
uniform mat4 u_directional_light_vp;

out vec2 v2f_uv;
out vec3 v2f_normal;
out vec4 v2f_frag_light_space_pos;
out vec3 v2f_frag_world_pos;

void main() {
    v2f_uv = in_uv;
    v2f_normal = normalize(mat3(transpose(inverse(u_model))) * in_normal);

    v2f_frag_world_pos = vec3(u_model * vec4(in_position, 1.0));
    v2f_frag_light_space_pos = u_directional_light_vp * vec4(v2f_frag_world_pos, 1.0);

    gl_Position = u_projection * u_view * u_model * vec4(in_position, 1.0);
};

#endif

//
// Fragment
//

#ifdef FRAGMENT

#define DIRECTIONAL_LIGHT_SAMPLES 1

in vec2 v2f_uv;
in vec3 v2f_normal;
in vec4 v2f_frag_light_space_pos;
in vec3 v2f_frag_world_pos;

uniform sampler2D u_texture;
uniform sampler2D u_shadowmap_directional;
uniform vec3 u_directional_light_dir;

out vec4 frag_color;

float get_t_shadow_directional() {
    vec3 light_dir = normalize(u_directional_light_dir);

    float alignment_with_directional_light = dot(v2f_normal, light_dir);
    if (alignment_with_directional_light < 0.0) {
        // Surface looking away from light is always in shadow
        return 1.0;
    }

    vec3 pos = v2f_frag_light_space_pos.xyz * 0.5 + 0.5; // Map from [-1,1] to [0,1]
    pos.z = min(pos.z, 1.0);

    // If the surface is perpendicular to the light direction
    // then it needs larger bias values
    // TODO @BACKLOG: Directional shadow with very narrow angles
    // This perp_bias isn't high enough to handle those cases. If we increase that
    // then we see the other artifacts and the base of the walls.
    float perp_bias = 0.001;
    float parallel_bias = 0.0001;
    float bias = max(perp_bias * (1.0 - alignment_with_directional_light), parallel_bias);

    float shadow = 0.0;

    float tex_depth = texture(u_shadowmap_directional, pos.xy).r; 
    shadow += (tex_depth + bias) < pos.z ? 1.0 : 0.0;  // 1 if shadowed
    return shadow;

//    vec2 texel_size = 1.0 / textureSize(u_shadowmap_directional, 0);
//    for (int x = -DIRECTIONAL_LIGHT_SAMPLES; x <= DIRECTIONAL_LIGHT_SAMPLES; x++) {
//        for (int y = -DIRECTIONAL_LIGHT_SAMPLES; y <= DIRECTIONAL_LIGHT_SAMPLES; y++) {
//            float pcf_depth = texture(u_shadowmap_directional, pos.xy + vec2(x, y) * texel_size).r; 
//            shadow += (pcf_depth + bias) < pos.z ? 1.0 : 0.0; // 1 if shadowed
//        }    
//    }
//    return shadow / ((DIRECTIONAL_LIGHT_SAMPLES + 2) * (DIRECTIONAL_LIGHT_SAMPLES + 2));
} 

void main() {
    float tmp = v2f_uv.x * v2f_normal.x * 0.00001f; // TODO @CLEANUP: Is this needed?

    float uv_max = max(v2f_uv.x, v2f_uv.y);
    vec2 tmp_uv_normalized = v2f_uv / uv_max;

    vec4 tex_color = texture(u_texture, v2f_uv);
    vec4 shadowed_tex_color = vec4(tex_color.rgb * 0.2, 1.0);

    float brightness = 0.0;
    float t = get_t_shadow_directional(); // 1 means shadow
    brightness += mix(1.0, 0.0, t);
    frag_color = mix(shadowed_tex_color, tex_color, brightness);
};
#endif
