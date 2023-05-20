#include "nodes/gui/GUINode.hpp"
#include "rendering/RenderingServer.hpp"
#include "rendering/Mesh.hpp"
#include "gui/GUITexture.hpp"
#include "utils/math.hpp"

using namespace llengine;

GUINode::GUINode(RenderingServer& rs) : rs(rs) {
    rs.register_gui_node(this);
}

GUINode::~GUINode() {
    rs.unregister_gui_node(this);
}

[[nodiscard]] glm::vec3 GUINode::get_screen_space_position() const {
    GUINode* parent = get_parent();
    const auto local_transform = get_transform();
    glm::vec3 local_position {
        local_transform.get_screen_space_offset(get_parent_size())
    };
    if (parent) {
        return local_position + parent->get_screen_space_position();
    }
    else {
        return local_position;
    }
}

[[nodiscard]] bool GUINode::contains_point(glm::vec2 point) const {
    glm::vec2 top_left = glm::vec2(get_screen_space_position());

    glm::vec2 bottom_right = top_left + get_absolute_size();

    return point.x >= top_left.x && point.x <= bottom_right.x &&
        point.y >= top_left.y && point.y <= bottom_right.y;
}

void GUINode::draw_children() {
    for (const auto& child_ptr : children) {
        child_ptr->draw();
    }
}

void GUINode::update_children() {
    for (const auto& child_ptr : children) {
        child_ptr->update();
    }
}

void GUINode::add_child(std::unique_ptr<GUINode>&& child) {
    child->parent = this;
    children.push_back(std::move(child));
}

void GUINode::draw_texture_part(
    const Texture& texture, glm::vec2 pos_offset_in_px, glm::vec2 tex_offset_in_px,
    glm::vec2 quad_size_in_px, glm::vec2 tex_part_size_in_px
) {
    const glm::vec2 viewport_size = rs.get_window().get_window_size();

    glm::vec2 position_in_px = get_screen_space_position();
    // The quad mesh is in range from -1.0 to 1.0, but
    // not from 0.0 to 1.0. So, to pivot it to the corner, add half of the size.
    position_in_px += pos_offset_in_px + quad_size_in_px / 2.0f;

    const glm::mat4 mvp = glm::translate(
        math_utils::scr_space_pos_to_gl_space(glm::vec3(position_in_px, get_transform().z_coordinate), viewport_size)
    ) * glm::scale(glm::vec3(quad_size_in_px / viewport_size, 1.0f));

    glm::vec2 uv_scale = tex_part_size_in_px / static_cast<glm::vec2>(texture.get_size());
    glm::vec2 uv_offset = tex_offset_in_px / static_cast<glm::vec2>(texture.get_size());
    uv_offset.y += uv_scale.y; // Invert UV's y. If (scale * y) goes from 0 to scale when y goes from 0 to 1,
    uv_scale.y = -uv_scale.y; // then (scale - scale * y) goes from scale to 0.

    rs.get_shader_holder().get_gui_rectangle_shader().use_shader(
        texture, mvp, uv_scale, uv_offset, {1.0f, 1.0f, 1.0f, 1.0f}
    );

    auto mesh = Mesh::get_quad();
    mesh->bind_vao(true, false, false);
    
    if (mesh->is_indexed()) {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->get_indices_id());
        glDrawElements(GL_TRIANGLES, mesh->get_amount_of_vertices(), mesh->get_indices_type(), 0);
    }
    else {
        glBindBuffer(GL_ARRAY_BUFFER, mesh->get_vertices_id());
        glDrawArrays(GL_TRIANGLES, 0, mesh->get_amount_of_vertices());
    }
    rs.report_about_drawn_triangles(mesh->get_amount_of_vertices() / 3);

    mesh->unbind_vao();
}

void GUINode::draw_rectangle(const GUITexture& texture) {
    const std::array<glm::vec2, 3> tex_part_sizes {
        glm::vec2(texture.left_border, texture.top_border),
        glm::vec2(texture->get_size().x - texture.left_border - texture.right_border, texture->get_size().y - texture.top_border - texture.bottom_border),
        glm::vec2(texture.right_border, texture.bottom_border)
    };
    const std::array<glm::vec2, 3> quad_sizes {
        glm::vec2(texture.left_border, texture.top_border),
        glm::vec2(get_absolute_size() - texture.left_border - texture.right_border),
        glm::vec2(texture.right_border, texture.bottom_border)
    };
    glm::vec2 offset_in_tex {0.0f, 0.0f};
    glm::vec2 offset_of_quad {0.0f, 0.0f};
    for (std::size_t x = 0; x < 3; x++) {
        for (std::size_t y = 0; y < 3; y++) {
            draw_texture_part(
                *texture,
                offset_of_quad,
                offset_in_tex,
                {quad_sizes[x].x, quad_sizes[y].y},
                {tex_part_sizes[x].x, tex_part_sizes[y].y}
            );
            offset_in_tex.y += tex_part_sizes[y].y;
            offset_of_quad.y += quad_sizes[y].y;
        }
        offset_in_tex.x += tex_part_sizes[x].x;
        offset_in_tex.y = 0.0f;
        offset_of_quad.x += quad_sizes[x].x;
        offset_of_quad.y = 0.0f;
    }
}

[[nodiscard]] glm::vec2 GUINode::get_parent_size() const {
    GUINode* parent = get_parent();
    if (parent) {
        return parent->get_absolute_size();
    }
    else {
        return rs.get_window().get_window_size();
    }
}