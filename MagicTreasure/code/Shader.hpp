//una clase shader generica pra todos los shaders(skybox, heightmap, base etc)
#pragma once

#include <string>
#include <glm.hpp>      // Para mat4, vec3
#include <glad/gl.h>    // Para GLuint

namespace udit
{
    class Shader
    {
    private:
        GLuint program_id; // El ID del programa

    public:
        // Constructor: Recibe EL CÓDIGO (strings)
        Shader(const std::string& vertex_code, const std::string& fragment_code);

        ~Shader();

        // Para activar el shader
        void use() const;

        // Getter
        GLuint get_id() const { return program_id; }

        void set_matrix(const std::string& name, const glm::mat4& matrix) const;
        void set_int(const std::string& name, int value) const;
        void set_float(const std::string& name, float value) const;
        void set_vec3(const std::string& name, const glm::vec3& value) const;

    private:
        //  funciones privadas de compilación
        GLuint compile_shaders(const std::string& vertex_shader_code, const std::string& fragment_shader_code);
        void show_compilation_error(GLuint shader_id);
        void show_linkage_error(GLuint program_id);
    };
}

