
// Este código es de dominio público
// angel.rodriguez@udit.es
// creación de la ventana y el contexto OpenGL dentro de una clase llamada Window


//cabeceras e includes
#pragma once //archivo no se incluye más de una vez

#include <cassert> //incluye herramienta assert pra detener el programa si errores
#include <glad/gl.h> //incluye GLAD para cargar funciones OpenGL en todos los sistemas
#include <SDL3/SDL_opengl.h> //incluye SDL para manejo de ventanas y contexto OpenGL
#include "Window.hpp"

namespace udit
{
 
    Window::Window
    (
		const char* title, //título de la ventana
		unsigned width, //ancho de la ventana
		unsigned height,  //alto de la ventana
		const OpenGL_Context_Settings & context_details //detalles del contexto OpenGL
    )
    {
        // Se hace inicializa SDL:

		if (not SDL_InitSubSystem(SDL_INIT_VIDEO)) //inicializa subsistema de video de SDL
        {
            throw "Failed to initialize the video subsystem.";
        }

        // Se preconfigura el contexto de OpenGL:

        SDL_GL_SetAttribute (SDL_GL_CONTEXT_MAJOR_VERSION, context_details.version_major); //pide la vrsion q venia en la configuracion
        SDL_GL_SetAttribute (SDL_GL_CONTEXT_MINOR_VERSION, context_details.version_minor);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, context_details.core_profile ? SDL_GL_CONTEXT_PROFILE_CORE : SDL_GL_CONTEXT_PROFILE_COMPATIBILITY); //perfil core moderno o aquel compatible
		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1); //activa doble buffer
		SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1); //pide una visualizacion acelerada por hardware

		if (context_details.depth_buffer_size) SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, context_details.depth_buffer_size); //si se pide buffer de profundidad, se configura pra saber q objetos estan delante o detras
		if (context_details.stencil_buffer_size) SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, context_details.stencil_buffer_size); //" una mascara pra efectos especiales (recortes / espejos etc)

        // Se crea la ventana activando el soporte para OpenGL:

        window_handle = SDL_CreateWindow
        (
            title,
            int(width ),
            int(height),
			SDL_WINDOW_OPENGL //bandera q indica q la ventana soporta OpenGL
        );

        assert(window_handle != nullptr);//window_handle es el puntero (la "llave" del hotel)

       
        // Se crea un contexto de OpenGL asociado a la ventana:

		opengl_context = SDL_GL_CreateContext(window_handle); //crea el hilo telefonico/ EL contexto/ con la GPU

        assert(opengl_context != nullptr);

        // Una vez se ha creado el contexto de OpenGL ya se puede inicializar GLAD:

		GLenum glad_is_initialized = gladLoaderLoadGL(); //tu programa no sabe dnd estan las funciones OpenGL, GLAD las carga

        assert(glad_is_initialized);

        // Se activa la sincronización con el refresco vertical del display:

		SDL_GL_SetSwapInterval(context_details.enable_vsync ? 1 : 0); //activa o desactiva VSync (sincronizacion vertical) entre la tarjeta grafica y el monitor pra misma velocidad de dibujado
    }

	Window::~Window() //destructor
    {
		gladLoaderUnloadGL(); //libera recursos cargados por GLAD

		if (opengl_context) //si existe contexto 
        {
			SDL_GL_DestroyContext(opengl_context); //destruye  liberando recursos asociados y Liberando la conexión con la GPU.
        }

        if (window_handle) //Si existe la ventana, la cierra y libera su memoria.
        {
            SDL_DestroyWindow (window_handle);
        }

		SDL_QuitSubSystem(SDL_INIT_VIDEO); //apaga el subsistema de video de SDL l
    }

    void Window::swap_buffers ()
    {
		SDL_GL_SwapWindow(window_handle); //intercambia el búfer trasero con el delantero, mostrando lo dibujado
    }
    //Back Buffer (Búfer Trasero/Oculto): Es un lienzo oculto en la memoria. 
    // Aquí es donde dibuja OpenGL. Cuando ejecutas comandos como
    // glClear (borrar) o glDrawArrays (dibujar triángulo)

}
