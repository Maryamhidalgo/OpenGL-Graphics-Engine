
#include "Texture.hpp"

namespace udit
{
	Texture::Texture()
		: id(0), width(0), height(0), type(GL_TEXTURE_2D)
	{
	}

	Texture::~Texture()
	{
		if (id != 0)
		{
			glDeleteTextures(1, &id);
			id = 0;
		}
	}

	void Texture::bind() const
	{
		if (id != 0)
		{
			glBindTexture(this->type, this->id);
		}
	}

	void Texture::unbind() const
	{
		glBindTexture(type, 0);
	}

	bool Texture::load_cubemap(const std::string& path_base)
	{
		bool texture_is_loaded = false;

		this->type = GL_TEXTURE_CUBE_MAP;

		using CubemapFormat = Rgba8888;

		std::vector<std::unique_ptr<Color_Buffer<CubemapFormat>>> texture_sides(6);

		for (size_t i = 0; i < 6; i++)
		{
			std::string full_path = path_base + std::to_string(i) + ".jpg";

			texture_sides[i] = load_image<CubemapFormat>(full_path);

			if (!texture_sides[i])
			{
				return false;
			}
		}

		glEnable(GL_TEXTURE_CUBE_MAP);

		glGenTextures(1, &this->id);
		glBindTexture(GL_TEXTURE_CUBE_MAP, this->id);

		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

		static const GLenum texture_target[] =
		{
			GL_TEXTURE_CUBE_MAP_NEGATIVE_Z,
			GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
			GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
			GL_TEXTURE_CUBE_MAP_POSITIVE_X,
			GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
			GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
		};

		for (size_t i = 0; i < 6; i++)
		{
			auto& texture = *texture_sides[i];

			glTexImage2D(
				texture_target[i],
				0,
				GL_RGBA,
				texture.get_width(),
				texture.get_height(),
				0,
				GL_RGBA,
				GL_UNSIGNED_BYTE,
				texture.colors()
			);
		}

		texture_is_loaded = true;
		return texture_is_loaded;
	}
}