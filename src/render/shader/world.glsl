#ifdef VERTEX
layout (location = 0) in vec3 in_position;
layout (location = 1) in vec2 in_uv;
layout (location = 2) in vec3 in_normal;

uniform mat4 u_model;
uniform mat4 u_view;
uniform mat4 u_projection;
uniform mat4 u_directional_light_vp;

out vec2 v2f_uv;
out vec3 v2f_world_normal;
out vec4 v2f_frag_light_space_pos;
out vec3 v2f_frag_world_pos;

#define JITTER_RESOLUTION vec2(160, 120)

void main() {
    v2f_uv = in_uv;

    // TODO @PERF: We can calculate world normals on CPU and send here afterwards (for static objects)
    v2f_world_normal = normalize(mat3(transpose(inverse(u_model))) * in_normal);

    v2f_frag_world_pos = vec3(u_model * vec4(in_position, 1.0));
    v2f_frag_light_space_pos = u_directional_light_vp * vec4(v2f_frag_world_pos, 1.0);

    vec4 clip_pos = u_projection * u_view * u_model * vec4(in_position, 1.0);

    // jitter effect
    clip_pos.xyz = clip_pos.xyz / clip_pos.w; // clip space -> NDC
    clip_pos.xy = floor(JITTER_RESOLUTION * clip_pos.xy) / JITTER_RESOLUTION;
    clip_pos.xyz *= clip_pos.w; // NDC -> clip space

    gl_Position = clip_pos;
};

#endif

//
// Fragment
//

#ifdef FRAGMENT

in vec2 v2f_uv;
in vec3 v2f_world_normal;
in vec4 v2f_frag_light_space_pos;
in vec3 v2f_frag_world_pos;

uniform sampler2D u_texture;
uniform sampler2D u_shadowmap_directional;
uniform vec3 u_directional_light_dir;
uniform vec3 u_directional_light_color;
#define DIRECTIONAL_LIGHT_SAMPLES 1

// Point light
#define MAX_LIGHT_COUNT 10
struct PointLight {
    vec3 position;
    vec3 color;
    float intensity;
    float attenuation;
};
uniform samplerCubeArray u_shadowmaps_point;
uniform PointLight u_point_lights[MAX_LIGHT_COUNT];
uniform int u_point_light_count;
uniform float u_far_plane;

out vec4 frag_color;

float get_directional_brightness() {
    vec3 light_dir = normalize(u_directional_light_dir);

    float alignment_with_directional_light = dot(v2f_world_normal, light_dir);
    if (alignment_with_directional_light < 0.0) {
        // Surface looking away from light is always in shadow
        return 0.0;
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

    float brightness = 0.0;

    vec2 texel_size = 1.0 / textureSize(u_shadowmap_directional, 0);
    for (int x = -DIRECTIONAL_LIGHT_SAMPLES; x <= DIRECTIONAL_LIGHT_SAMPLES; x++) {
        for (int y = -DIRECTIONAL_LIGHT_SAMPLES; y <= DIRECTIONAL_LIGHT_SAMPLES; y++) {
            float pcf_depth = texture(u_shadowmap_directional, pos.xy + vec2(x, y) * texel_size).r; 
            brightness += (pcf_depth + bias) < pos.z ? 0.0 : 1.0;
        }    
    }
    return brightness / ((DIRECTIONAL_LIGHT_SAMPLES + 2) * (DIRECTIONAL_LIGHT_SAMPLES + 2));
} 

vec3 point_shadow_sample_offset_directions[20] = vec3[] (
   vec3( 1,  1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1,  1,  1), 
   vec3( 1,  1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1,  1, -1),
   vec3( 1,  1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1,  1,  0),
   vec3( 1,  0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1,  0, -1),
   vec3( 0,  1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0,  1, -1)
);  

#define POINT_SHADOW_SOFTNESS_WITH_DISTANCE 0.005

float get_frag_brightness_from_light(PointLight light, int light_index) {
    vec3 frag_to_point_light = light.position - v2f_frag_world_pos;
    float alignment_with_point_light = max(dot(v2f_world_normal, normalize(frag_to_point_light)), 0.0);
    if (alignment_with_point_light < 0) {
        return 0; // Looking away from point light
    }

    float brightness = 0.0;

    vec3 light_to_frag = v2f_frag_world_pos - light.position;
    float light_to_frag_dist = length(light_to_frag) - 0.0005; // bias

    // Larger sample distance as the fragment is further away from the light
    float disk_radius = POINT_SHADOW_SOFTNESS_WITH_DISTANCE * light_to_frag_dist;
    for (int i = 0; i < 20; i++) {
        vec3 sample_dir = light_to_frag + point_shadow_sample_offset_directions[i] * disk_radius;
        float depth_in_cubemap = texture(u_shadowmaps_point, vec4(sample_dir, light_index)).r;
        depth_in_cubemap *= u_far_plane;
        if (abs(light_to_frag_dist - depth_in_cubemap) < 0.01) {
            brightness += 1.0;
        }
    }
    brightness = brightness / 20.0;

    float distance_to_point_light = length(frag_to_point_light);

    float max_brightness = alignment_with_point_light * light.intensity
        / (light.attenuation * distance_to_point_light);

    return mix(0.0, max_brightness, brightness);
}

void main() {
    float brightness = mix(0.0, 1.0, get_directional_brightness());
    vec3 total_light_color = u_directional_light_color * brightness;

    for (int i = 0; i < u_point_light_count; i++) {
        float point_light_brightness = get_frag_brightness_from_light(u_point_lights[i], i);
        brightness += point_light_brightness;
        total_light_color += u_point_lights[i].color * point_light_brightness;
    }

    vec4 tex_color = texture(u_texture, v2f_uv);
    tex_color *= vec4(normalize(total_light_color), 1.0);
    vec4 shadowed_tex_color = vec4(tex_color.rgb * 0.05, 1.0);

    frag_color = mix(shadowed_tex_color, tex_color, brightness);
};
#endif
