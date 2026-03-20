#include "Material.hpp"
#include "Shader.hpp"
#include "Texture.hpp"
#include <glad/gl.h> // Necesario para GL_TEXTURE0, glActiveTexture..
namespace udit 
{
	 //Método principal: Configura el shader antes de dibujar
		void Material:: apply() const
		{
			if (!shader_program) return;//si no hay shader program asignado, no hace nada

			// 1. Activar el Shader
			shader_program->use();//hace internamente glUseProgram

			// Enviar Parámetros (Uniforms)
			for (const auto& [name, value] : parameters)
			{
				
				std::visit([&, this, key = name](auto&& arg) //función lambda
				{
						this->set_uniform_value(key, arg);//pasamos referencias al método set_shader_value
				}, value);

			
			}

			// Enviar Texturas
			int texture_unit = 0;//unidad de textura inicial (GL_TEXTURE0)
			for (const auto& [name, texture] : textures)
			{
				if (texture)
				{
					// Activamos la unidad 
					glActiveTexture(GL_TEXTURE0 + texture_unit);

					// Vinculamos la textura
					texture->bind();

					// Decimos al shader que el sampler "name" lea de la unidad "texture_unit"
					shader_program->set_int(name, texture_unit);

					texture_unit++;
				}
			}

			// Restaurar a unidad 0 por buenas prácticas
			glActiveTexture(GL_TEXTURE0);
		}

		//Helpers para llamar a las funciones del Shader

		void Material::set_uniform_value(const std::string& name, int val) const
		{
			shader_program->set_int(name, val);
		}

		void Material::set_uniform_value(const std::string& name, float val) const
		{
			shader_program->set_float(name, val);
		}

		void Material::set_uniform_value(const std::string& name, const glm::vec3& val) const
		{
			shader_program->set_vec3(name, val);
		}

		
}
