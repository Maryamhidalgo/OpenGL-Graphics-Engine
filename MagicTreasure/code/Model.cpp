
#include "Model.hpp"
#include <glm.hpp>

namespace udit
{
	void Model::add_piece(const std::string& name, std::shared_ptr<Mesh> m, std::shared_ptr<Material> mat)
	{
		pieces[name] = { m, mat };
	}

	void Model::render(const Camera& camera)
	{
		glm::mat4 view_matrix = camera.get_transform_matrix_inverse();
		glm::mat4 proj_matrix = camera.get_projection_matrix();
		glm::mat4 model_matrix = this->get_transform();

		glm::mat4 normal_matrix = glm::transpose(glm::inverse(model_matrix));
		normal_matrix[3] = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

		for (auto& [name, piece] : pieces)
		{
			if (!piece.mesh || !piece.material || !piece.material->shader_program) continue;

			auto shader = piece.material->shader_program;

			piece.material->apply();

			shader->set_matrix("u_model", model_matrix);
			shader->set_matrix("u_view", view_matrix);
			shader->set_matrix("u_projection", proj_matrix);
			shader->set_matrix("u_normal_matrix", normal_matrix);

			piece.mesh->draw_mesh();
		}

		Actor::render(camera);
	}
}