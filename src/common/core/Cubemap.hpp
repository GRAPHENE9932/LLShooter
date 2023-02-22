#pragma once

#include <memory> // std::shared_ptr

#include <GL/glew.h> // GLuint

class Texture;
class RenderingServer;

class Cubemap {
public:
    Cubemap(const Cubemap& other) = delete;
    Cubemap(Cubemap&& other) noexcept;
    ~Cubemap();

    static Cubemap from_cubemap(RenderingServer& rs, const std::shared_ptr<Texture>& cubemap_texture);
    static Cubemap from_panorama(RenderingServer& rs, const std::shared_ptr<Texture>& panorama_texture);

    const Texture& get_texture() {
        return *cubemap_texture;
    }

    void draw();

private:
    static GLuint vertices_id;
    static inline std::size_t amount_of_sky_boxes = 0;
    std::shared_ptr<Texture> cubemap_texture;
    RenderingServer& rendering_server;

    Cubemap(RenderingServer& rs, const std::shared_ptr<Texture>& cubemap_texture);

    static void static_init_if_needed();
    static void static_clean_up();
};
