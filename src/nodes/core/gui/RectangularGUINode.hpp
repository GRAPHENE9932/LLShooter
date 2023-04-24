#pragma once

#include "GUINode.hpp"
#include "common/core/GUITexture.hpp"

#include <glm/mat4x4.hpp>

class GUITexture;

class RectangularGUINode : public GUINode {
public:
    enum class OriginX : std::uint8_t {
        LEFT, CENTER, RIGHT
    };
    enum class OriginY : std::uint8_t {
        BOTTOM, CENTER, TOP
    };

    RectangularGUINode(RenderingServer& rs) : GUINode(rs) {}

    void set_origin_x(OriginX new_origin_x) noexcept {
        origin_x = new_origin_x;
    }
    void set_origin_y(OriginY new_origin_y) noexcept {
        origin_y = new_origin_y;
    }

    [[nodiscard]] virtual glm::vec2 get_size() const = 0;
    /**
     * @brief Returns position of the origin (according to origin_x and origin_y)
     * relatively to the node position.
     * 
     * @return glm::vec2 
     */
    [[nodiscard]] virtual glm::vec2 get_origin() const;
    /**
     * @param point The point position in screen (pixel) coordinates.
     * (top left is 0, 0).
     */
    [[nodiscard]] bool contains_point(glm::vec2 point) const;

protected:
    OriginX origin_x = OriginX::LEFT;
    OriginY origin_y = OriginY::BOTTOM;

    void draw_rectangle(const GUITexture& texture);

private:
    void draw_texture_part(
        const Texture& texture, glm::vec2 pos_offset_in_px, glm::vec2 tex_offset_in_px,
        glm::vec2 quad_size_in_px, glm::vec2 tex_part_size_in_px
    );
};