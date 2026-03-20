
// Este código es de dominio público
// angel.rodriguez@udit.es

#ifndef SKYBOX_HEADER
#define SKYBOX_HEADER
#include "Model.hpp"
#include <string>

namespace udit
{
        class Skybox : public Model
        {
        public:
            // Constructor: Carga recursos (texturas )y crea el shader, la malla y el material
            Skybox(const std::string& texture_path);

            // Sobrescribimos render para el truco de profundidad y vista
            void render(const Camera& camera) override;
        };    
}

#endif
