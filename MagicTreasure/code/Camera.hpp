// Este código es de dominio público
// angel.rodriguez@udit.es

//Evita inclusiones múltiples de este archivo
#ifndef CAMERA_HEADER
#define CAMERA_HEADER

// Incluye GLM (OpenGL Mathematics) para operaciones matemáticas, vectores y matrices.
#include <glm.hpp>                          // vec3, vec4, ivec4, mat4
#include <gtc/matrix_transform.hpp>         // translate, rotate, scale, perspective
#include <gtc/type_ptr.hpp>                 // value_ptr

namespace udit
{

    class Camera
    {
        //Usa vec4 (4 componentes: x, y, z, w) para puntos y vectores, lo cual facilita la multiplicación directa por matrices de 4x4.
        using Point = glm::vec4;
        using Vector = glm::vec4;
        using Matrix44 = glm::mat4;

        //Miembros privados de la clase Camera - estados y propiedades de la cámara
    private:

        float    fov;//zoom o campo de visión en grados
        float    near_z;// distancia minima de renderizado (+ cerca no se dibuja)
        float    far_z; // distancia maxima de renderizado (+ lejos no se dibuja)
        float    ratio; // relación de aspecto (ancho/alto) de la ventana o viewport

        Point    location;//dnd esta
        Point    target;//hacia donde mira

        Matrix44 projection_matrix;//matriz de proyección (perspectiva) Almacena la matriz matemática que convierte coordenadas 3D en coordenadas 2D de pantalla.

    public:
        //constructor basico de la cámara con valores predeterminados
        Camera(float ratio = 1.f)//solo ratio
        {
            reset(60.f, 0.1f, 1000.f, ratio);//valores por defecto 60 grados fov, 0.1 near z, 1000 far z
        }
        //Otros constructores
        Camera(float near_z, float far_z, float ratio = 1.f)
        {
            reset(60.f, near_z, far_z, ratio);
        }

        Camera(float fov_degrees, float near_z, float far_z, float ratio)
        {
            reset(fov_degrees, near_z, far_z, ratio);
        }

    public:
        //Getters para acceder a las propiedades de la cámara SOLO LECTURA
        float         get_fov() const { return fov; }
        float         get_near_z() const { return near_z; }
        float         get_far_z() const { return far_z; }
        float         get_ratio() const { return ratio; }

        //const: Prometen no modificar el objeto.
        //Devuelven referencias(&) en los vectores para ser más eficientes y no copiar datos innecesariamente.
        const Point& get_location() const { return location; }
        const Point& get_target() const { return target; }

    public:
        //Cuando cambias una propiedad óptica (como el fov o el ratio), automáticamente llama a calculate_projection_matrix(). 
        // Esto actualiza la matriz interna para que el cambio surta efecto visual.
        void set_fov(float new_fov) { fov = new_fov;    calculate_projection_matrix(); }
        void set_near_z(float new_near_z) { near_z = new_near_z; calculate_projection_matrix(); }
        void set_far_z(float new_far_z) { far_z = new_far_z;  calculate_projection_matrix(); }
        void set_ratio(float new_ratio) { ratio = new_ratio;  calculate_projection_matrix(); }

        //Establecen manualmente la posición o el punto de mira.
        void set_location(float x, float y, float z) { location[0] = x; location[1] = y; location[2] = z; }
        void set_target(float x, float y, float z) { target[0] = x; target[1] = y; target[2] = z; }

        //Resetear la cámara a una posición y orientación predeterminadas.
        void reset(float new_fov, float new_near_z, float new_far_z, float new_ratio)
        {
            set_fov(new_fov);
            set_near_z(new_near_z);
            set_far_z(new_far_z);
            set_ratio(new_ratio);
            set_location(0.f, 0.f, 0.f);
            set_target(0.f, 0.f, -1.f);// mira hacia el fondo, eje Z negativo
            calculate_projection_matrix();
        }

    public:
        //Desplaza la cámara (Panning). Mueve tanto la posición como el objetivo en la misma dirección.
        void move(const glm::vec3& translation)
        {
            location += glm::vec4(translation, 1.f);
            target += glm::vec4(translation, 1.f);
        }
        //Rota la cámara alrededor de su posición actual. Solo afecta al objetivo.
        void rotate(const glm::mat4& rotation)
        {
            target = location + rotation * (target - location);
            //   --Suma el vector rotado a la posición actual para encontrar el nuevo punto target  -- Aplica la matriz de rotación a ese vector dirección. -- Calcula el vector de dirección desde la cámara hacia el objetivo.
        }

    public:
        //Obtención de Matrices (Para el Shader)
        //Devuelve la matriz de proyección (perspectiva) calculada internamente.
        const glm::mat4& get_projection_matrix() const
        {
            return projection_matrix;
        }

        //Devuelve la matriz de vista (transformación inversa) que posiciona y orienta la cámara en el mundo 3D.
        glm::mat4 get_transform_matrix_inverse() const
        {
            /*Se llama _inverse porque mover la cámara a la derecha equivale matemáticamente a mover todo el mundo a la izquierda(inverso).

            glm::lookAt: Es una función auxiliar mágica de GLM que construye la matriz necesaria dados :

            Posición del ojo(location).

             Hacia dónde mira(target).

            Qué es "arriba" (el vector 0, 1, 0 o eje Y).*/

            return glm::lookAt
            (
                glm::vec3(location[0], location[1], location[2]),
                glm::vec3(target[0], target[1], target[2]),
                glm::vec3(0.0f, 1.0f, 0.0f)
            );
        }

    private:
        //Calcula la matriz de proyección perspectiva basada en el fov, ratio, near_z y far_z actuales.
        void calculate_projection_matrix()
        {
            //Convierte el fov de grados a radianes porque GLM trabaja en radianes.
            projection_matrix = glm::perspective(glm::radians(fov), ratio, near_z, far_z);
        }

    };

}

#endif

