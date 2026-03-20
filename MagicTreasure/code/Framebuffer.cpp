#include "Framebuffer.hpp"
namespace udit
{
	Framebuffer::Framebuffer(int w, int h)
		: width(w), height(h), framebuffer_id(0), out_texture_id(0), depthbuffer_id(0)
	{
		// Se crea un framebuffer en el que poder renderizar:
		{
			glGenFramebuffers(1, &framebuffer_id);
			glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_id);
		}
		// Se crea una textura que será el búffer de color vinculado al framebuffer:
		{
			glGenTextures(1, &out_texture_id);
			glBindTexture(GL_TEXTURE_2D, out_texture_id);

			// El búfer de color tendrá formato RGB:

			glTexImage2D
			(
				GL_TEXTURE_2D,
				0,
				GL_RGB,
				width,
				height,
				0,
				GL_RGB,
				GL_UNSIGNED_BYTE,
				0
			);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		}
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, out_texture_id, 0);// Vincular textura como búffer de color sin datos iniciales
		
		
		// Se crea un Z-Buffer para usarlo en combinación con el framebuffer:
		{
			glGenRenderbuffers(1, &depthbuffer_id);
			glBindRenderbuffer(GL_RENDERBUFFER, depthbuffer_id);
			glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
			glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthbuffer_id);
		}
		// Comprobar si el Framebuffer está completo
		 // Se comprueba que el framebuffer está listo:

		assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);

		// Se desvincula el framebuffer:

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

	}

	Framebuffer::~Framebuffer()
	{
		glDeleteFramebuffers(1, &framebuffer_id);
		glDeleteTextures(1, &out_texture_id);
		glDeleteRenderbuffers(1, &depthbuffer_id);
	}
	void Framebuffer::bind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_id);
		glViewport(0, 0, width, height);
	}

	void Framebuffer::unbind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void Framebuffer::resize(int new_width, int new_height)
	{
		width = new_width;
		height = new_height;
		// Redimensionar la textura de salida:
		glBindTexture(GL_TEXTURE_2D, out_texture_id);
		glTexImage2D
		(
			GL_TEXTURE_2D,
			0,
			GL_RGB,
			width,
			height,
			0,
			GL_RGB,
			GL_UNSIGNED_BYTE,
			0
		);
		// Redimensionar el depthbuffer:
		glBindRenderbuffer(GL_RENDERBUFFER, depthbuffer_id);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
	}
}