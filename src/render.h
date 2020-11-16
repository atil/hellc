#ifndef _RENDER_H_
#define _RENDER_H_

#include <glm/glm.hpp>

int load_shader_program(const char* shader_path);
void set_int(int shader_program_id, const char* property_name, int i);
void set_vec3(int shader_program_id, const char* property_name, glm::vec3 v);
void set_mat4(int shader_program_id, const char* property_name, glm::mat4 m);
void set_float(int shader_program_id, const char* property_name, float f);
void delete_shader_program(int shader_program_id);

#endif
