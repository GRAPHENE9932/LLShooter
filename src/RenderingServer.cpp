#include <cstring>
#include <stdexcept>

#include "objects/BitmapTextObject.hpp"
#include "objects/UnshadedDrawableObject.hpp"
#include "objects/TexturedDrawableObject.hpp"
#include "objects/ImageObject.hpp"
#include "RenderingServer.hpp"

RenderingServer::RenderingServer(int window_width, int window_height) {
    init_window(window_width, window_height);
    init_gl();
}

void RenderingServer::set_update_callback(std::function<void(float)> func) {
    update_callback = func;
}

GLFWwindow* RenderingServer::get_window() {
    return window;
}

void RenderingServer::add_drawable_object(std::shared_ptr<DrawableObject> obj, const bool overlay) {
    auto& cur_vector {overlay ? drawable_objects_overlay : drawable_objects};

    auto iter {std::lower_bound(
        cur_vector.begin(), cur_vector.end(), obj,
        [] (const std::shared_ptr<DrawableObject>& obj_1, const std::shared_ptr<DrawableObject>& obj_2) {
            return obj_1->get_program_id() < obj_2->get_program_id();
        }
    )};

    cur_vector.insert(iter, obj);
}

void RenderingServer::init_window(int window_width, int window_height) {
    int glfw_init_res = glfwInit();

    if (!glfw_init_res) {
        throw std::runtime_error(
            std::string("Failed to initialize GLFW. Error code: ") + std::to_string(glfw_init_res)
        );
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(window_width, window_height, "LLShooter", nullptr, nullptr);
    if (window == nullptr)
        throw std::runtime_error("Failed to create window.");

    glfwMakeContextCurrent(window);
    //glfwSwapInterval(0);

    int glew_init_res = glewInit();
    if (glew_init_res != GLEW_OK) {
        throw std::runtime_error(
            std::string("Failed to initialize GLEW. Error code: " + std::to_string(glew_init_res))
        );
    }
}

void RenderingServer::init_gl() {
    // Create the VAO.
    GLuint vertex_array_id;
    glGenVertexArrays(1, &vertex_array_id);
    glBindVertexArray(vertex_array_id);

    // Enable depth test.
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    // Set blend function.
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Enable backface culling.
    glEnable(GL_CULL_FACE);
}

void RenderingServer::main_loop() {
    TexturedDrawableObject::pre_init();
    UnshadedDrawableObject::pre_init();
    ImageObject::pre_init();
    BitmapTextObject::pre_init();

    glClearColor(0.0f, 0.0f, 0.1f, 0.0f);

    prev_frame_time = std::chrono::high_resolution_clock::now();
    do {
        // Measure time and callback.
        auto now = std::chrono::high_resolution_clock::now();
        auto duration = now - prev_frame_time;
        prev_frame_time = now;
        float delta = std::chrono::duration_cast<std::chrono::duration<float>>(duration).count();
        update_callback(delta);

        // Prepare draw parameters.
        draw_params.view_matrix = camera->compute_view_matrix();
        const glm::mat4 proj_matrix = camera->get_proj_matrix();
        draw_params.view_proj_matrix = proj_matrix * draw_params.view_matrix;
        for (GLuint i = 0; i < POINT_LIGHTS_AMOUNT; i++)
            draw_params.point_lights[i].overlay_position =
                    draw_params.view_matrix * glm::vec4(draw_params.point_lights[i].position, 1.0f);

        glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

        draw_params.overlay_mode = false;
        for (std::size_t i = 0; i < drawable_objects.size(); i++)
            drawable_objects[i]->draw(draw_params);

        glClear(GL_DEPTH_BUFFER_BIT);

        // Overlay objects are already relative to view, so they don't
        // need the view matrix in their model-view-projection matrix.
        draw_params.view_proj_matrix = proj_matrix;
        draw_params.overlay_mode = true;
        for (std::size_t i = 0; i < drawable_objects_overlay.size(); i++)
            drawable_objects_overlay[i]->draw(draw_params);

        glfwSwapBuffers(window);
        glfwPollEvents();
    } while (!glfwWindowShouldClose(window));
}
