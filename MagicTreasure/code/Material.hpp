#pragma once

#include <map>
#include <string>
#include <variant>
#include <memory>
#include <glm.hpp>      // Para vec3, vec4
#include <gtc/type_ptr.hpp> // value_ptr punteros de matrices y vectores glm
#include "Shader.hpp"
#include "Texture.hpp"
using namespace std;
using namespace glm;
//Define el aspecto (color, brillo, textura).
namespace udit
{
	class Material
	{
	public:
		using Value = std::variant<int, float, glm::vec3, glm::vec4>; //tipo de dato personalizado llamado Value. std::variant: Es una característica de C++17 que puede almacenar int, float, vec3 o vec4

		// Puntero al Shader Program que usará este material
		std::shared_ptr<Shader> shader_program;
		// Mapa de propiedades del material
		std::map<std::string, Value> parameters;//nombre, valor

		// Mapa de texturas (Nombre del sampler -> Objeto Textura)
		std::map<std::string, std::shared_ptr<Texture>> textures;
	public:
		Material() = default;

		// Método principal de configuracion del shader antes llamando a los helpers internos
		void apply() const;

	private:
		// Helpers para llamar a las funciones del Shader
		void set_uniform_value(const std::string& name, int val) const;
		void set_uniform_value(const std::string& name, float val) const;
		void set_uniform_value(const std::string& name, const glm::vec3& val) const;
		
	};
}