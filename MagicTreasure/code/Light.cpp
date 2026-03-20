#include "Light.hpp"
namespace udit
{
    Light::Light(const glm::vec3& c, float ambient, float diffuse)
        : color(c),
        ambient_intensity(ambient),
        diffuse_intensity(diffuse)
    {
    }
}