#include "Terrain.hpp"
#include <vector>
#include <iostream>

using glm::vec3;
using std::vector;
namespace udit
{
    // --- SHADERS ADAPTADOS A TU CLASE MESH ---

    // Vertex Shader:
    // - Layout 0 (aPos): Tu Mesh envía vec3. Usaremos x y z.
    // - Layout 2 (aTexCoord): Tu Mesh envía las UVs aquí.
    //const std::string terrain_v =
    //    "#version 330 core\n"
    //    "layout (location = 0) in vec3 aPos;"      // Coincide con Mesh::COORDINATES_VBO
    //    "layout (location = 2) in vec2 aTexCoord;" // Coincide con Mesh::TEXTURES_VBO
    //    ""
    //    "uniform mat4 u_model;"      // Tu Model::render envía esto
    //    "uniform mat4 u_view;"       // Tu Model::render envía esto
    //    "uniform mat4 u_projection;" // Tu Model::render envía esto
    //    ""
    //    "uniform sampler2D heightMap;" // La textura de altura
    //    "uniform float     max_height;"
    //    ""
    //    "out float intensity;"
    //    "out vec2  TexCoord;"
    //    ""
    //    "void main()"
    //    "{"
    //    "    // Leemos la altura (rojo) de la textura"
    //    "    float h = texture(heightMap, aTexCoord).r;"
    //    ""
    //    "    // Elevamos el vértice en Y"
    //    "    vec3 pos = aPos;"
    //    "    pos.y = h * max_height;"
    //    ""
    //    "    // Calculamos intensidad para el color (zonas altas mas claras)"
    //    "    intensity = h * 0.7 + 0.3;"
    //    "    TexCoord  = aTexCoord;"
    //    ""
    //    "    gl_Position = u_projection * u_view * u_model * vec4(pos, 1.0);"
    //    "}";
    

    //const std::string terrain_v =

    //    "#version 330core\n"
    //    ""
    //    // IMPORTANTE: Nombres estándar de tu Model::render
    //    "uniform mat4 u_model;"
    //    "uniform mat4 u_view;"
    //    "uniform mat4 u_projection;"
    //    ""
    //    "layout (location = 0) in vec3 aPos;" //in: Datos de entrada de cada vértice.Solo recibe X y Z (plano), porque la Y se calcula aquí.
    //    "layout (location = 2) in vec2 aTexCoord;" //Coordenadas de textura para saber qué píxel de la imagen leer.
    //    ""
    //    "uniform sampler2D heightMap;" //La textura del mapa de altura. uniform: Significa "Este dato viene desde la CPU, sampler2D: Es una textura 2D.sampler es el nombre de la variable en el shader.
    //    "uniform float     max_height;" //Altura máxima que puede alcanzar el terreno.
    //    ""
    //    "out float intensity;"//Una variable que enviaremos al Fragment Shader para colorear el terreno según su altura.
    //    "out vec2  TexCoord;"
    //    ""
    //    "void main()"
    //    "{"
    //    "   float h = texture (heightMap, aTexCoord).r;"//Lee el componente Rojo de la imagen en la posición UV. Como es una imagen en blanco y negro, el rojo representa el brillo (0.0 negro, 1.0 blanco).
    //    "   intensity    = h * 0.75 + 0.25;"//Calcula un color base para que las zonas bajas sean oscuras y las altas claras.
    //    "    TexCoord  = aTexCoord;"
    //    "   float height = h * max_height;" //Multiplica el brillo por max_height. Si el píxel es blanco, la altura será 5.0 (por ejemplo). Si es negro, será 0.
    //    "   vec4 xyzw = vec4(aPos.x, height, aPos.z, 1.0);" //xyzw: Construye la posición 3D real insertando la height calculada en el eje Y.
    //    "   gl_Position  = u_projection * u_view * u_model * xyzw;" //gl_Position: Transforma el punto 3D a coordenadas de pantalla 2D.
    //    "}";
    
    // --- SHADER CORREGIDO Y MÁS ROBUSTO ---
    const std::string terrain_v =
        "#version 330 core\n" 
        "layout (location = 0) in vec3 aPos;"
        "layout (location = 2) in vec2 aTexCoord;"
        ""
        "uniform mat4 u_model;"
        "uniform mat4 u_view;"
        "uniform mat4 u_projection;"
        ""
        "uniform sampler2D samplers[2];"
        "uniform float     max_height;"
        ""
        "out float intensity;"
        "out vec2  TexCoord;"
        ""
        "void main()"
        "{"
        "    float h = textureLod(samplers[0], aTexCoord, 0.0).r;"
        ""
        "    intensity = h * 0.75 + 0.25;"
        "    TexCoord  = aTexCoord;"
        ""
        "    float height = h * max_height;"
        "    vec4 xyzw = vec4(aPos.x, height, aPos.z, 1.0);"
        ""
        "    gl_Position = u_projection * u_view * u_model * xyzw;"
        "}";

    // Fragment Shader:
    const std::string terrain_f =
        "#version 330 core\n"
        "in float intensity;"
        "in vec2  TexCoord;"
        "out vec4 FragColor;"
        "uniform sampler2D samplers[2];"
        ""
        "void main()"
        "{"
        "    vec4 texColor = texture(samplers[1], TexCoord);"
        "    FragColor = texColor * intensity;"
        "}";

    Terrain::Terrain(float max_height, float width, float depth, unsigned x_slices, unsigned z_slices, const std::string& heightmap_path, const std::string& texture_path)
    {
        // 1. GENERACIÓN DE LA MALLA
		//para reservar espacio en los vectores
        GLsizei  number_of_vertices = x_slices * z_slices;// Número total de vértices de la malla filas + columnas del modelo terrain
		
		// Vectores para guardar los datos de la malla
        vector<float> vertices;
        vertices.reserve(number_of_vertices * 3);

        vector<float> uvs;
        uvs.reserve(number_of_vertices * 2);
        vector<unsigned int> indices;

        //// Calcula los "pasos" (steps): cuánto hay que avanzar en cada iteración del bucle para colocar el siguiente vértice.
        float x_step = width / float(x_slices);
        float z_step = depth / float(z_slices);
        float u_step = 1.f / float(x_slices);
        float v_step = 1.f / float(z_slices);

        // Centramos el terreno en (0,0)
        float x_start = -width / 2.0f;
        float z_start = -depth / 2.0f;

        // Bucle para generar vértices y UVs
        for (unsigned z = 0; z <= z_slices; ++z)
        {
            for (unsigned x = 0; x <= x_slices; ++x)
            {
                // Posición (X, 0, Z) -> La Y se calcula en el shader
                vertices.push_back(x_start + x * x_step); // X
                vertices.push_back(0.0f);                 // Y
                vertices.push_back(z_start + z * z_step); // Z

                // UVs (de 0.0 a 1.0)
                uvs.push_back((float)x / x_slices); // U
                uvs.push_back((float)z / z_slices); // V
            }
        }

        // Bucle para generar índices (TRIANGULOS)
        // Tu Mesh usa glDrawElements(GL_TRIANGLES), así que necesitamos 2 triángulos por cuadrado
        for (unsigned z = 0; z < z_slices; ++z)
        {
            for (unsigned x = 0; x < x_slices; ++x)
            {
                unsigned int top_left = z * (x_slices + 1) + x;
                unsigned int top_right = top_left + 1;
                unsigned int bottom_left = (z + 1) * (x_slices + 1) + x;
                unsigned int bottom_right = bottom_left + 1;

                // Triángulo 1
                indices.push_back(top_left);
                indices.push_back(bottom_left);
                indices.push_back(top_right);

                // Triángulo 2
                indices.push_back(top_right);
                indices.push_back(bottom_left);
                indices.push_back(bottom_right);
            }
        }

		// Crear la malla usando los datos generados
        auto terrain_mesh = std::make_shared<Mesh>();
        // create_mesh espera normales, pasamos vector vacío {} porque el shader no las usa (usa altura)
        terrain_mesh->create_mesh(vertices, {}, uvs, indices);

		// Crear el material con el shader personalizado
        auto terrain_shader = std::make_shared<Shader>(terrain_v, terrain_f);
        

        // 2. CONFIGURACIÓN DEL MATERIAL Y TEXTURAS
		//Crear la textura  altura
        auto texture_heigh = std::make_shared<Texture>();
        // Importante: map de alturas suele ser R8 o Monocromo, pero Rgba8888 funciona siempre
        //texture->create_texture_2d<Monochrome8>(heightmap_path);
		texture_heigh->create_texture_2d<Rgba8888>(heightmap_path);

        //textura de color
        auto texture_color = std::make_shared<Texture>();
        texture_color->create_texture_2d<Rgba8888>(texture_path);

		//Crear el material
        auto material = std::make_shared<Material>();
        material->shader_program = terrain_shader;

		//Establecemos los parámetros del material
		//Shader y su float de altura para la textura de alturas
        material->textures["samplers[0]"] = texture_heigh; // Coincide con 'uniform sampler2D sampler[0]' Internamente hace shader->set_int("samplers[0]", unidad_0)
        material->textures["samplers[1]"] = texture_color;
        material->parameters["max_height"] = max_height; // Altura máxima (puedes cambiarla) 

        // 4. AÑADIR AL MODELO
        this->add_piece("surface", terrain_mesh, material);
    }
}