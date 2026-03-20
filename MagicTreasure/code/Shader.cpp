#include "Shader.hpp"
#include <vector>
#include <iostream>
#include <cassert>
#include <SDL3/SDL.h>//incluye contexto OpenG
#include <gtc/type_ptr.hpp> // value_ptr punteros de matrices y vectores glm

using namespace std;

namespace udit
{
	Shader::Shader(const std::string& vertex_code, const std::string& fragment_code)
	{
		program_id = 0;
		program_id = compile_shaders(vertex_code, fragment_code);
	}
    Shader::~Shader()
    {
        if (program_id != 0) glDeleteProgram(program_id);
    }

    void Shader::use() const
    {
        if (program_id != 0) glUseProgram(program_id);
    }


    //SETTERS (Asignación de datos al shader)
    void Shader::set_matrix(const std::string& name, const glm::mat4& matrix) const
    {
		GLint MatrixLocation = glGetUniformLocation(program_id, name.c_str());//busca la ubicación del uniform matrix en el shader por su nombre
		if (MatrixLocation != -1) glUniformMatrix4fv(MatrixLocation, 1, GL_FALSE, glm::value_ptr(matrix));//si lo encuentra le pasa el valor de la matriz.
		//tipo de dato, ubicación, cantidad, transponer la matriz?, puntero a los datos
    }

    void Shader::set_int(const std::string& name, int value) const //x ejemplo para unidad de textura. lee ranura 0 o 1 etc
    {
        GLint Iloc = glGetUniformLocation(program_id, name.c_str());
        if (Iloc != -1) glUniform1i(Iloc, value);
    }

    void Shader::set_float(const std::string& name, float value) const
    {
        GLint Floc = glGetUniformLocation(program_id, name.c_str());
        if (Floc != -1) glUniform1f(Floc, value);
    }

    void Shader::set_vec3(const std::string& name, const glm::vec3& value) const
    {
        GLint V3loc = glGetUniformLocation(program_id, name.c_str());
        if (V3loc != -1) glUniform3fv(V3loc, 1, glm::value_ptr(value));
    }

    GLuint Shader::compile_shaders(const std::string& vertex_shader_code, const std::string& fragment_shader_code)
    {
        GLint succeeded = GL_FALSE;

        GLuint vertex_shader_id = glCreateShader(GL_VERTEX_SHADER);
        GLuint fragment_shader_id = glCreateShader(GL_FRAGMENT_SHADER);

        // --- PARTE A: Preparación de código flexible ---
        const char* vertex_shaders_code[] = { vertex_shader_code.c_str() };
        const char* fragment_shaders_code[] = { fragment_shader_code.c_str() };
        const GLint vertex_shaders_size[] = { (GLint)vertex_shader_code.size() };
        const GLint fragment_shaders_size[] = { (GLint)fragment_shader_code.size() };

        glShaderSource(vertex_shader_id, 1, vertex_shaders_code, vertex_shaders_size);
        glShaderSource(fragment_shader_id, 1, fragment_shaders_code, fragment_shaders_size);

        // Compilación
        glCompileShader(vertex_shader_id);
        glCompileShader(fragment_shader_id);

        //Comprobacion
        glGetShaderiv(vertex_shader_id, GL_COMPILE_STATUS, &succeeded);
        if (!succeeded) show_compilation_error(vertex_shader_id);

        glGetShaderiv(fragment_shader_id, GL_COMPILE_STATUS, &succeeded);
        if (!succeeded) show_compilation_error(fragment_shader_id);

        //Linkeo
        GLuint program_id = glCreateProgram();

        glAttachShader(program_id, vertex_shader_id);
        glAttachShader(program_id, fragment_shader_id);

        glLinkProgram(program_id);

        glGetProgramiv(program_id, GL_LINK_STATUS, &succeeded);
        if (!succeeded) show_linkage_error(program_id);

		//Limpieza
        glDeleteShader(vertex_shader_id);
        glDeleteShader(fragment_shader_id);

        return program_id;
    }

    void Shader::show_compilation_error(GLuint shader_id)
    {
        string info_log;
        GLint  info_log_length;

        glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &info_log_length);
        info_log.resize(info_log_length);
        glGetShaderInfoLog(shader_id, info_log_length, NULL, &info_log.front());

        cerr << info_log.c_str() << endl;
        assert(false);
    }

    void Shader::show_linkage_error(GLuint program_id)
    {
        string info_log;
        GLint  info_log_length;

        glGetProgramiv(program_id, GL_INFO_LOG_LENGTH, &info_log_length);
        info_log.resize(info_log_length);
        glGetProgramInfoLog(program_id, info_log_length, NULL, &info_log.front());

        cerr << info_log.c_str() << endl;
        assert(false);
    }
}