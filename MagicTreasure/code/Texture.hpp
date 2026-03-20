//Distingue entre texturas normales (RGBA) y mapas de altura (Monochrome) usando plantillas
//Las funciones con template deben ir en el .hpp
//Tiene que diferenciar textura tipo 2D y CubeMap pra skybox
#pragma once

#include <string>
#include <memory>
#include <iostream>
#include <glad/gl.h>
#include <SOIL2.h>
#include <algorithm> // Necesario para std::copy_n

#include "Color.hpp"
#include "Color_Buffer.hpp"

namespace udit
{
    class Texture
    {
    private:
        GLuint   id;
        unsigned width;
        unsigned height;
        GLenum type; // GL_TEXTURE_2D o GL_TEXTURE_CUBE_MAP

    public:
        Texture();
        ~Texture();

        // Evitar copias
        Texture(const Texture&) = delete;
        Texture& operator=(const Texture&) = delete;

        // Getters
        GLuint   get_id() const { return id; }
        unsigned get_width() const { return width; }
        unsigned get_height() const { return height; }

        void bind() const;
        void unbind() const;

        bool load_cubemap(const std::string& texture_path);//para caso Skybox

		//Carga una imagen desde un archivo con SOIL y devuelve un Color_Buffer con el formato adecuado
        template< typename COLOR_FORMAT >
        std::unique_ptr< Color_Buffer< COLOR_FORMAT > > load_image(const std::string& image_path)
        {
            // Se carga la imagen del archivo:

            int image_width = 0;
            int image_height = 0;
            int image_channels = 0;
            // queremos cargar Color (4 canales) o Mapa de Altura (1 canal)
            int soil_channels = (sizeof(typename Color_Buffer<COLOR_FORMAT>::Color) == 1) ? SOIL_LOAD_L : SOIL_LOAD_RGBA;

            uint8_t* loaded_pixels = SOIL_load_image
            (
                image_path.c_str(),
                &image_width,
                &image_height,
                &image_channels,
                soil_channels
            );

            // Si loaded_pixels no es nullptr, la imagen se ha podido cargar correctamente:

            if (loaded_pixels)
            {
                // Actualizamos las dimensiones de la clase Texture
                this->width = image_width;
                this->height = image_height;

                auto image = std::make_unique< Color_Buffer< COLOR_FORMAT > >(image_width, image_height);

                // Se copian los bytes de un buffer a otro directamente:

                std::copy_n
                (
                    loaded_pixels,
                    size_t(image_width) * size_t(image_height) * sizeof(COLOR_FORMAT),
                    reinterpret_cast<uint8_t*>(image->colors())
                );

                // Se libera la memoria que reservó SOIL2 para cargar la imagen:

                SOIL_free_image_data(loaded_pixels);

                return image;
            }

            return nullptr;
        }



        //funcion es una plantilla devuelve un int. Usa el template para saber cómo interpretar los bytes (si leer 1 byte por píxel o 4).
        template< typename COLOR_FORMAT >
        GLuint create_texture_2d(const std::string& texture_path)
        {
            auto image = load_image< COLOR_FORMAT >(texture_path);

            if (image)
            {
                GLuint texture_id;

                glEnable(GL_TEXTURE_2D); // Activa texturizado 2D
                glGenTextures(1, &texture_id);//genera un ID pra la textura
                glBindTexture(GL_TEXTURE_2D, texture_id);//binding pra q cualquier comando de configuración de texturas que yo escriba, aplícaselo a la textura con el ID texture_id

                // Configuración de parámetros 
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); 
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
              
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

               
                GLenum internal_format;
                GLenum format;

                if (sizeof(typename Color_Buffer<COLOR_FORMAT>::Color) == 1)
                {
                    // Caso MONOCHROME8 (Mapa de altura)
                    internal_format = GL_R8;
                    format = GL_RED;
                }
                else
                {
                    // Caso RGBA8888 (Color normal)
                    internal_format = GL_RGBA8;
                    format = GL_RGBA;
                }

                glTexImage2D
                (
                    GL_TEXTURE_2D,
                    0,
                    internal_format,
                    image->get_width(),
                    image->get_height(),
                    0,
                    format,
                    GL_UNSIGNED_BYTE,
                    image->colors()
                );

                glGenerateMipmap(GL_TEXTURE_2D);
                
                this->id = texture_id;
                return texture_id;
            }

            return -1; 
        }

       
    
    };
  

}


