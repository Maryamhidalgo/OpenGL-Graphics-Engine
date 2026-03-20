#pragma once

#include <glad/gl.h>
#include <iostream>
#include <cassert>
namespace udit
{
	class Framebuffer 
	{
	private:
		GLuint framebuffer_id;
		GLuint depthbuffer_id;
		GLuint out_texture_id;

		int width;
		int height;
	public:
		Framebuffer(int width, int height);
		~Framebuffer();

		// Vincula el FBO para escribir en él (Paso 1 del render)
		void bind();

		// Desvincula el FBO para volver a escribir en la pantalla (Paso 2 del render)
		void unbind();

		// Obtiene el ID de la textura de salida (para leerla en el paso 2 del render)
		GLuint get_output_texture() const { return out_texture_id; }

		// Por si la ventana cambia de tamaño
		void resize(int new_width, int new_height);
	};
}