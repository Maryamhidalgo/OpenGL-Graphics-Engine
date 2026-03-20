#pragma once
#include "Actor.hpp"
#include <glm.hpp>

namespace udit
{
    class Light : public Actor
    {
    public:
        // Propiedades de la luz
        glm::vec3 color;
        float ambient_intensity;
        float diffuse_intensity;

        // Constructor con parámetros (y valores por defecto)
        Light(const glm::vec3& color, float ambient , float diffuse);

        virtual ~Light() = default;
    };
}
