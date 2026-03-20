#pragma once
#include "Model.hpp"
#include <string>
#include <glad/gl.h> // GLuint, GLsizei
namespace udit
{
	class Terrain: public Model
	{
 
    public:
    
        Terrain(float max_height, float width, float depth, unsigned x_slices, unsigned z_slices, const std::string& heightmap_path, const std::string& texture_path);

        virtual ~Terrain() = default;

	};
}


