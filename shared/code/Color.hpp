
// Este código es de dominio público
// angel.rodriguez@udit.es
//Define tipos de datos para representar colores en diferentes formatos.

#pragma once

#include <cstdint>//uint8_t, uint32_t pra  tipos de datos enteros de tamaño fijo

namespace udit
{
	// Define un color RGBA con 8 bits por canal (32 bits en total) uso para mapas de altura.
    using Monochrome8 = uint8_t;//Define un alias. En lugar de escribir uint8_t, puedes escribir Monochrome8.

	//union todos los componentes de color en una sola variable de 32 bits(msmo espacio de memoria)
    union Rgba8888
    {
        enum { RED, GREEN, BLUE, ALPHA };

		uint32_t value; //Valor entero de 32 bits que representa el color completo.
		uint8_t  components[4];//Array de 4 bytes, uno para cada componente de color (R, G, B, A).
    };

}
