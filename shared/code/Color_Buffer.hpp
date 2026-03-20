
// Este código es de dominio público
// angel.rodriguez@udit.es
//Implementa una clase genérica para manejar un búfer de colores en memoria. (imagen o textura)
//es un contenedor que guarda los píxeles de una imagen
#pragma once

#include <vector>

namespace udit
{
    /*Ejemplo: Color_Buffer<Monochrome8> creará un buffer de bytes, 
     mientras que Color_Buffer<Rgba8888>  creará un buffer de enteros de 32 bits.*/
    template< typename COLOR >
    class Color_Buffer
    {
    public:

		using Color = COLOR;//Define un alias para el tipo de color utilizado en el búfer.

    private:
		//Dimensiones del búfer de colores:
        unsigned width;
        unsigned height;

        std::vector< Color > buffer;//El vector contendrá un total de width * height elementos.

    public:

        //El vector contendrá un total de width * height elementos.
        Color_Buffer(unsigned width, unsigned height) 
        :
			//recibe el ancho y alto del buffer
            width (width ), 
            height(height),
            buffer(width * height)//reserva memoria
        {
        }
		//devuelve el amcho y alto del buffer
        unsigned get_width () const
        {
            return width;
        }

        unsigned get_height () const
        {
            return height;
        }

		//devuelve un puntero al primer elemento del buffer
        Color * colors ()
        {
            return buffer.data ();//buffeer.data() acesoo a la memoria cruda
        }

        const Color * colors () const
        {
            return buffer.data ();
        }

		//Acceso a píxeles individuales en el búfer mediante un índice lineal
        //Devuelve una referencia (&). Esto es muy eficiente porque no copia el píxel. 
        // Además, en la versión no-const, te permite modificarlo directamente:
        Color & get (unsigned offset)//offset: Es la posición lineal del píxel. Si la imagen es de 10x10, el offset va de 0 a 99.
        {
            return buffer[offset];
        }

        const Color & get (unsigned offset) const
        {
            return buffer[offset];
        }

		//Permite modificar el color de un píxel en la posición dada por offset:
        void set (unsigned offset, const Color & color)
        {
            buffer[offset] = color;
        }

    };

}
