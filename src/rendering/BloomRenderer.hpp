#pragma once

#include "datatypes.hpp"
#include "BloomFramebuffer.hpp"
#include "rendering/shaders/GaussianBlurShader.hpp"

#include <glm/vec2.hpp>

#include <span>

namespace llengine {
class BloomRenderer {
public:
    BloomRenderer(glm::u32vec2 framebuffer_size);
    ~BloomRenderer();

    void assign_framebuffer_size(glm::u32vec2 framebuffer_size);
    void render_to_bloom_texture(const std::vector<Texture>& source_texture_lods, float bloom_radius);
    [[nodiscard]] TextureID get_bloom_texture_id() const;

private:
    BloomFramebuffer framebuffer;
    glm::u32vec2 framebuffer_size;
    GaussianBlurShader blur_shader;
    TextureID result_texture_id = 0;

    std::uint32_t image_stages = 4;

    void do_blur(
        std::span<const Texture> source_texture_lods,
        std::span<const Texture> target_texture_lods,
        float blur_radius,
        bool is_vertical
    );
    void combine();
};
}