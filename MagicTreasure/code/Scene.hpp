
#pragma once

#include <memory>
#include <string>
#include <map>          
#include <glad/gl.h>

#include "Camera.hpp"
#include "Skybox.hpp"
#include "Terrain.hpp"
#include "Light.hpp"
#include "Model.hpp"
#include "Texture.hpp"
#include "Shader.hpp"
#include "Mesh.hpp"
#include "Material.hpp"
#include "Framebuffer.hpp"

namespace udit
{
    class Scene
    {
    private:
        Camera camera;

        // ESTRUCTURA PRINCIPAL(padres)
        // Raíces del grafo para las pasadas de renderizado
        std::shared_ptr<Model> root;   // Opacos
        std::shared_ptr<Model> rootT;  // Transparentes

		//Mapa de modelos cargados en la escena
        std::map<std::string, std::shared_ptr<Model>> models;

		//otros objetos de la escena que no son modelos
        std::shared_ptr<Skybox> skybox;
        std::shared_ptr<Terrain> terrain;
        std::shared_ptr<Light> light;
        std::shared_ptr<Shader> shader;
        std::shared_ptr<Texture> texture;

		// objetos para post-procesado
        std::shared_ptr<Framebuffer> framebuffer;
        std::shared_ptr<Shader> screen_shader;
        std::shared_ptr<Mesh> screen_quad;

        // control de rotacion cristales
        float orbit_angle = 0.0f;
        

        int width, height;
        float angle_around_x = 0.0f, angle_around_y = 0.0f;
        float angle_delta_x = 0.0f, angle_delta_y = 0.0f;
        float last_pointer_x = 0.0f, last_pointer_y = 0.0f;
        bool  pointer_pressed = false;

    public:
        Scene(int width, int height);
        ~Scene() = default;
   
        void update();
        void render();
        void resize(int width, int height);
        void on_drag(float x, float y);
        void on_click(float x, float y, bool down);

    private:
        void load_resources();
        void setup_screen_quad();
    };
}