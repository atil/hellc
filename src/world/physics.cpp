#include "world.h"

constexpr glm::vec3 unit_y(0, 1, 0);

struct PlayerShape {
	glm::vec3 up {};
	glm::vec3 bottom {};
	float radius;

	explicit PlayerShape(const glm::vec3& player_pos, float height, float radius): radius(radius) {
		this->up = player_pos + (unit_y * (height / 2));
		this->bottom = player_pos - (unit_y * (height / 2));
	}
};

void Physics::register_obj(const ObjModelData& obj_data) {

}

void Physics::tick(glm::vec3& player_pos, float dt) const {
	PlayerShape player_shape(player_pos, 1, 0.5f);

	// TODO: Collision resolving here
}
