//Usa la librería Assimp para leer archivos 3D
//Contiene VAO/VBO/ EVO para guardar los datos de carga
//Renderizado (Draw): Activa el VAO y lanza la orden de dibujo (glDrawElements) para que la tarjeta gráfica pinte la malla.
#pragma once
#include <memory>
#include <string>
#include <glad/gl.h>
#include <glm.hpp>
#include <fstream>
//Define la geometría
namespace udit 
{
    using glm::vec3;
	class Mesh
	{
	private:
        enum
        {
            COORDINATES_VBO,
            NORMALS_VBO, // Location 1 en Shader (¡Vital para luz!)
            TEXTURES_VBO,
            INDICES_EBO,
            VBO_COUNT
        };

    private:
        GLuint vbo_ids[VBO_COUNT];
        GLuint vao_id;
        GLsizei number_of_indices;

    public:
        // Constructor y Destructor
        Mesh();
        ~Mesh();

        // Deshabilitar copia para evitar borrar buffers dos veces (RAII)
        Mesh(const Mesh&) = delete;
        Mesh& operator=(const Mesh&) = delete;

        // Métodos principales
        bool load_mesh(const std::string& file_path);
        // --- NUEVO MÉTODO PARA GENERAR MALLAS MANUALMENTE ---> x ejemplo pra terrenos elevados
        // Recibe los vectores de datos directamente en lugar de leer un archivo
        bool create_mesh(const std::vector<float>& vertices, const std::vector<float>& normals,const std::vector<float>& uvs, const std::vector<unsigned int>& indices);
        void draw_mesh() const;

        // Getters útiles (opcional)
        size_t get_index_count() const { return number_of_indices; }

	};
}


