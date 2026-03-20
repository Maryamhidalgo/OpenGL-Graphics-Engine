
#include "Skybox.hpp"
#include <vector>

using namespace std;
using namespace glm;

namespace udit
{
	const std::string sky_v =
		"#version 330\n"
		"uniform mat4 model_view_matrix;"
		"uniform mat4 projection_matrix;"
		"layout (location = 0) in vec3 vertex_coordinates;"
		"out vec3 texture_coordinates;"
		"void main()"
		"{"
		"    texture_coordinates = vec3(vertex_coordinates.x, -vertex_coordinates.y, vertex_coordinates.z);"
		"    gl_Position = projection_matrix * model_view_matrix * vec4(vertex_coordinates, 1.0);"
		"}";

	const std::string sky_f =
		"#version 330\n"
		"in vec3 texture_coordinates;"
		"out vec4 fragment_color;"
		"uniform samplerCube sampler;"
		"void main()"
		"{"
		"    fragment_color = texture(sampler, texture_coordinates);"
		"}";

	Skybox::Skybox(const std::string& texture_path)
	{
		auto Skyshader = std::make_shared<Shader>(sky_v, sky_f);

		auto texture = std::make_shared<Texture>();
		texture->load_cubemap(texture_path);

		auto material = std::make_shared<Material>();
		material->shader_program = Skyshader;
		material->textures["sampler"] = texture;

		auto mesh = std::make_shared<Mesh>();

		vector<float> coordinates = {
			-1.0f,  1.0f, -1.0f, -1.0f, -1.0f, -1.0f,  1.0f, -1.0f, -1.0f,
			 1.0f, -1.0f, -1.0f,  1.0f,  1.0f, -1.0f, -1.0f,  1.0f, -1.0f,
			-1.0f, -1.0f,  1.0f, -1.0f, -1.0f, -1.0f, -1.0f,  1.0f, -1.0f,
			-1.0f,  1.0f, -1.0f, -1.0f,  1.0f,  1.0f, -1.0f, -1.0f,  1.0f,
			 1.0f, -1.0f, -1.0f,  1.0f, -1.0f,  1.0f,  1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,  1.0f,  1.0f, -1.0f,  1.0f, -1.0f, -1.0f,
			-1.0f, -1.0f,  1.0f, -1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,  1.0f, -1.0f,  1.0f, -1.0f, -1.0f,  1.0f,
			-1.0f,  1.0f, -1.0f,  1.0f,  1.0f, -1.0f,  1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f, -1.0f,  1.0f,  1.0f, -1.0f,  1.0f, -1.0f,
			-1.0f, -1.0f, -1.0f, -1.0f, -1.0f,  1.0f,  1.0f, -1.0f, -1.0f,
			 1.0f, -1.0f, -1.0f, -1.0f, -1.0f,  1.0f,  1.0f, -1.0f,  1.0f
		};

		std::vector<unsigned int> indices;
		for (unsigned int i = 0; i < 36; i++) indices.push_back(i);

		mesh->create_mesh(coordinates, {}, {}, indices);

		this->add_piece("sky", mesh, material);
	}

	void Skybox::render(const Camera& camera)
	{
		glm::mat4 view = camera.get_transform_matrix_inverse();
		view[3] = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

		glDepthMask(GL_FALSE);

		for (auto& [name, piece] : pieces)
		{
			if (!piece.material) continue;

			auto shader = piece.material->shader_program;
			shader->use();
			piece.material->apply();

			shader->set_matrix("model_view_matrix", view);
			shader->set_matrix("projection_matrix", camera.get_projection_matrix());

			piece.mesh->draw_mesh();
		}

		glDepthMask(GL_TRUE);

		Actor::render(camera);
	}
}