#pragma once

#include <vector>
#include <string>
#include <memory>
#include <glm.hpp>
#include "Camera.hpp"

namespace udit
{
	using glm::vec3;

	class Actor
	{
	public:
		using Id = uint64_t;
		using Ptr = std::shared_ptr<Actor>;

		static Id id_counter;

		// --- Constructor & Destructor ---
		Actor()
		{
			id = id_counter++;
		}
		virtual ~Actor() = default;

		// --- Getters ---
		Id get_id() const
		{
			return id;
		}

		const std::string& get_name() const { return name; }
		const glm::mat4& get_transform() const { return global_transform; }
		vec3 get_position() const { return position; }

		// --- Setters ---
		void set_position(const glm::vec3& pos) { position = pos; }
		void set_rotation(const glm::vec3& rot) { rotation = rot; }
		void set_scale(const glm::vec3& s) { scale = s; }

		// --- Jerarquía ---
		void add_child(Ptr child);
		virtual void update_hierarchy(const glm::mat4& parent_matrix);

		// --- Render ---
		virtual void render(const Camera& camera)
		{
			for (auto& child : children) {
				child->render(camera);
			}
		}

		
		std::vector<Ptr> children;

	protected:
		
		std::string name;

		glm::vec3 position{ 0.0f, 0.0f, 0.0f };
		glm::vec3 rotation{ 0.0f, 0.0f, 0.0f };
		glm::vec3 scale{ 1.0f, 1.0f, 1.0f };

		glm::mat4 global_transform{ 1.0f };

		Actor* parent = nullptr;

	private:
	
		Id id;
	};
}