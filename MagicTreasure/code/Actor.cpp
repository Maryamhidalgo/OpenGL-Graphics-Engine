#include "Actor.hpp"

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

using namespace std;
using namespace glm;

namespace udit
{
	Actor::Id Actor::id_counter = 0;

	void Actor::add_child(Ptr child)
	{
		child->parent = this;
		children.push_back(child);
	}

	void Actor::update_hierarchy(const glm::mat4& parent_matrix)
	{
		glm::mat4 local = glm::mat4(1.0f);

		local = glm::translate(local, position);

		local = glm::rotate(local, glm::radians(rotation.x), glm::vec3(1, 0, 0));
		local = glm::rotate(local, glm::radians(rotation.y), glm::vec3(0, 1, 0));
		local = glm::rotate(local, glm::radians(rotation.z), glm::vec3(0, 0, 1));

		local = glm::scale(local, scale);

		global_transform = parent_matrix * local;

		for (auto& child : children)
		{
			child->update_hierarchy(global_transform);
		}
	}
}