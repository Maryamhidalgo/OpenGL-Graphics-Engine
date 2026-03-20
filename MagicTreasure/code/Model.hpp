#pragma once

#include <map>
#include <string>
#include <memory>

#include "Actor.hpp"    // Heredamos de tu clase base
#include "Mesh.hpp"
#include "Material.hpp" // Tu clase Material
#include "Camera.hpp"   // Tu clase Camera
namespace udit
{
	class Model : public Actor
	{
    public:
        // Estructura interna: Una pieza es Malla + Material
        struct Piece
        {
            std::shared_ptr<Mesh> mesh;
            std::shared_ptr<Material> material;
        };
    protected:
        // Mapa de piezas (ej: "rueda", "chasis"...)
        std::map<std::string, Piece> pieces;
        //Mapas en vez de vectores pq Da igual el orden en el  q se han guardado, tú buscas por su nombre

    public:
        // Heredamos los constructores de Actor 
        using Actor::Actor;

        virtual ~Model() = default;

        // Añadir geometría + aspecto al modelo
        void add_piece(const std::string& name, std::shared_ptr<Mesh> m, std::shared_ptr<Material> mat);

		// renderizado: Recorre todas las piezas y las dibuja
        void render(const Camera& camera) override;

	};
}


