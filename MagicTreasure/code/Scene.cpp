
#include "Scene.hpp"
#include <iostream>
#include <vector>
#include <cmath>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>

const std::string vertex_src =
"#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"layout (location = 1) in vec3 aNormal;\n"
"layout (location = 2) in vec2 aTexCoord;\n"
"\n"
"uniform mat4 u_model;\n"
"uniform mat4 u_view;\n"
"uniform mat4 u_projection;\n"
"uniform mat4 u_normal_matrix;\n"
"\n"
"out vec3 v_FragPos;\n"
"out vec3 v_Normal;\n"
"out vec2 v_TexCoord;\n"
"\n"
"void main() {\n"
"    v_FragPos = vec3(u_model * vec4(aPos, 1.0));\n"
"    v_Normal = mat3(u_normal_matrix) * aNormal;\n"
"    v_TexCoord = aTexCoord;\n"
"    gl_Position = u_projection * u_view * vec4(v_FragPos, 1.0);\n"
"}";

const std::string fragment_src =
"#version 330 core\n"
"in vec3 v_FragPos;\n"
"in vec3 v_Normal;\n"
"in vec2 v_TexCoord;\n"
"\n"
"out vec4 FragColor;\n"
"\n"
"uniform sampler2D u_texture;\n"
"\n"
"uniform vec3  u_light_pos;\n"
"uniform vec3  u_light_color;\n"
"uniform float u_ambient_intensity;\n"
"uniform float u_diffuse_intensity;\n"
"uniform float u_opacity;\n"
"\n"
"void main() {\n"
"    vec4 texColor = texture(u_texture, v_TexCoord);\n"
"    vec3 norm = normalize(v_Normal);\n"
"    vec3 lightDir = normalize(u_light_pos - v_FragPos);\n"
"    float diff = max(dot(norm, lightDir), 0.0);\n"
"\n"
"    float mixFactor = diff * 0.7;\n"
"    vec3 litAppearance = mix(texColor.rgb, u_light_color, mixFactor);\n"
"\n"
"    vec3 ambient = u_ambient_intensity * texColor.rgb;\n"
"    vec3 diffuse = u_diffuse_intensity * diff * litAppearance;\n"
"    vec3 result = ambient + diffuse;\n"
"\n"
"    FragColor = vec4(result, texColor.a * u_opacity);\n"
"}";

const std::string screen_vertex_src =
"#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"layout (location = 2) in vec2 aTexCoords;\n"
"out vec2 TexCoords;\n"
"void main() {\n"
"    TexCoords = aTexCoords;\n"
"    gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0);\n"
"}";

const std::string screen_fragment_gray_src =
"#version 330 core\n"
"out vec4 FragColor;\n"
"in vec2 TexCoords;\n"
"uniform sampler2D screenTexture;\n"
"void main() {\n"
"    vec4 color = texture(screenTexture, TexCoords);\n"
"    float gray = dot(color.rgb, vec3(0.299, 0.587, 0.114));\n"
"    FragColor = vec4(gray, gray, gray, 1.0);\n"
"}";

namespace udit
{
    Scene::Scene(int width, int height) : width(width), height(height)
    {
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);

        root = std::make_shared<Model>();
        rootT = std::make_shared<Model>();
        root->set_position({ 0.0f, 0.0f, 0.0f });
        rootT->set_position({ 0.0f, 0.0f, 0.0f });

        load_resources();
    }

    void Scene::load_resources()
    {
        framebuffer = std::make_shared<Framebuffer>(width, height);

        screen_shader = std::make_shared<Shader>(screen_vertex_src, screen_fragment_gray_src);
        screen_shader->use();
        screen_shader->set_int("screenTexture", 0);

        setup_screen_quad();

        light = std::make_shared<Light>(glm::vec3(1.0f, 0.0f, 0.8f), 0.75f, 0.8f);
        light->set_position({ -17.0f, 3.5f, -10.0f });
        root->add_child(light);

        shader = std::make_shared<Shader>(vertex_src, fragment_src);
        texture = std::make_shared<Texture>();
        texture->create_texture_2d<Rgba8888>("../../../shared/assets/Crystal.png");

        auto texture2 = std::make_shared<Texture>();
        texture2->create_texture_2d<Rgba8888>("../../../shared/assets/treasure.png");

        auto material_treasure = std::make_shared<Material>();
        material_treasure->shader_program = shader;
        material_treasure->textures["u_texture"] = texture2;
        material_treasure->parameters["u_opacity"] = 1.0f;

        auto material_crystal = std::make_shared<Material>();
        material_crystal->shader_program = shader;
        material_crystal->textures["u_texture"] = texture;
        material_crystal->parameters["u_opacity"] = 0.7f;

        auto mesh_treasure = std::make_shared<Mesh>();
        if (mesh_treasure->load_mesh("../../../shared/assets/treasure.fbx"))
        {
            auto treasure = std::make_shared<Model>();
            treasure->add_piece("main_treasure", mesh_treasure, material_treasure);

            root->add_child(treasure);
            models["treasure"] = treasure;

            treasure->set_position({ -0.5f, -1.5f, -12.0f });
            treasure->set_rotation({ -90.0f, 0.0f, 0.0f });
            treasure->set_scale({ 0.04f, 0.04f, 0.04f });
        }

        auto mesh = std::make_shared<Mesh>();
        if (mesh->load_mesh("../../../shared/assets/Crystal.fbx"))
        {
            auto pivot = std::make_shared<Model>();
            rootT->add_child(pivot);
            models["pivot"] = pivot;

            int num_crystals = 4;
            float radius = 4.0f;

            for (int i = 0; i < num_crystals; i++)
            {
                auto crystal = std::make_shared<Model>();
                crystal->add_piece("main_crystal", mesh, material_crystal);

                float angle_rad = (i * (360.0f / num_crystals)) * (3.14159f / 180.0f);
                float x = radius * cos(angle_rad);
                float z = radius * sin(angle_rad);

                crystal->set_position({ x, 2.0f, z });
                crystal->set_scale({ 0.3f, 0.3f, 0.3f });

                pivot->add_child(crystal);

                std::string name = "crystal_" + std::to_string(i);
                models[name] = crystal;
            }
            pivot->set_position({ 0.0f, 0.0f, -15.0f });
        }
        else
        {
            std::cerr << "Error cargando Crystal.fbx" << std::endl;
        }

        terrain = std::make_shared<Terrain>(6.0f, 60.0f, 60.0f, 100, 100, "../../../shared/assets/height-map.png", "../../../shared/assets/grass.jpg");
        root->add_child(terrain);
        skybox = std::make_shared<Skybox>("../../../shared/assets/space-");
    }

    void Scene::update()
    {
        terrain->set_position({ 0.0f, -5.0f, -10.0f });

        orbit_angle += 0.5f;

        if (models.count("pivot"))
        {
            auto pivot = models["pivot"];
            pivot->set_rotation({ 0.0f, orbit_angle, 0.0f });
        }

        static float altura_actual = 2.0f;
        static float direccion = 1.0f;
        float velocidad = 0.05f;

        altura_actual += velocidad * direccion;

        if (altura_actual > 3.5f) {
            altura_actual = 3.5f;
            direccion = -1.0f;
        }
        else if (altura_actual < 0.5f) {
            altura_actual = 0.5f;
            direccion = 1.0f;
        }

        for (auto& [name, model] : models)
        {
            if (name.find("crystal_") != std::string::npos)
            {
                glm::vec3 pos_local = model->get_position();
                model->set_position({ pos_local.x, altura_actual, pos_local.z });
            }
        }

        if (root)  root->update_hierarchy(glm::mat4(1.0f));
        if (rootT) rootT->update_hierarchy(glm::mat4(1.0f));

        angle_around_x += angle_delta_x;
        angle_around_y += angle_delta_y;
        if (angle_around_x < -1.5f) angle_around_x = -1.5f;
        if (angle_around_x > +1.5f) angle_around_x = +1.5f;

        glm::mat4 cam_rot(1.0f);
        cam_rot = glm::rotate(cam_rot, angle_around_y, glm::vec3(0, 1, 0));
        cam_rot = glm::rotate(cam_rot, angle_around_x, glm::vec3(1, 0, 0));

        camera.set_target(0, 0, -1);
        camera.rotate(cam_rot);
    }

    void Scene::render()
    {
        bool active_post_process = (int(orbit_angle / 20.0f) % 2) == 0;

        if (active_post_process && framebuffer)
        {
            framebuffer->bind();
        }
        else
        {
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glViewport(0, 0, width, height);
        }

        glEnable(GL_DEPTH_TEST);
        glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if (skybox) skybox->render(camera);

        if (shader && light)
        {
            shader->use();
            glm::vec3 lightPos = glm::vec3(light->get_transform()[3]);
            shader->set_vec3("u_light_pos", lightPos);
            shader->set_vec3("u_light_color", light->color);
            shader->set_float("u_ambient_intensity", light->ambient_intensity);
            shader->set_float("u_diffuse_intensity", light->diffuse_intensity);
        }

        if (root)
        {
            glDisable(GL_BLEND);
            glDepthMask(GL_TRUE);
            root->render(camera);
        }

        if (rootT)
        {
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glDepthMask(GL_FALSE);
            rootT->render(camera);
            glDepthMask(GL_TRUE);
            glDisable(GL_BLEND);
        }

        if (active_post_process && framebuffer)
        {
            framebuffer->unbind();

            glDisable(GL_DEPTH_TEST);
            glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);

            if (screen_shader && screen_quad)
            {
                screen_shader->use();

                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, framebuffer->get_output_texture());

                screen_quad->draw_mesh();
            }
        }
    }

    void Scene::setup_screen_quad()
    {
        screen_quad = std::make_shared<Mesh>();

        std::vector<float> vertices = {
            -1.0f,  1.0f, 0.0f,
            -1.0f, -1.0f, 0.0f,
             1.0f,  1.0f, 0.0f,
             1.0f, -1.0f, 0.0f
        };

        std::vector<float> uvs = {
            0.0f, 1.0f,
            0.0f, 0.0f,
            1.0f, 1.0f,
            1.0f, 0.0f
        };

        std::vector<unsigned int> indices = { 0, 1, 2, 2, 1, 3 };

        screen_quad->create_mesh(vertices, {}, uvs, indices);
    }

    void Scene::resize(int w, int h)
    {
        width = w;
        height = h;
        camera.set_ratio((float)width / height);
        glViewport(0, 0, width, height);

        if (framebuffer) framebuffer->resize(w, h);
    }

    void Scene::on_drag(float x, float y)
    {
        if (pointer_pressed)
        {
            angle_delta_x = 0.025f * (last_pointer_y - y) / height;
            angle_delta_y = 0.025f * (last_pointer_x - x) / width;
        }
    }

    void Scene::on_click(float x, float y, bool down)
    {
        if ((pointer_pressed = down))
        {
            last_pointer_x = x;
            last_pointer_y = y;
        }
        else
        {
            angle_delta_x = angle_delta_y = 0.0f;
        }
    }
}