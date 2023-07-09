#pragma once

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

#include "nodes/CompleteSpatialNode.hpp"

namespace llengine {
class RenderingServer;

/**
 * @brief Basic static camera. Everything it needs - spatial parameters,
 * field of view and display ratio.
 */
class CameraNode : public CompleteSpatialNode {
public:
    CameraNode();

    glm::mat4 get_view_matrix() noexcept;
    glm::mat4 get_proj_matrix() noexcept;

    void set_translation(const glm::vec3& new_trans) override;
    void translate(const glm::vec3& translation) override;
    void set_rotation(const glm::quat& new_rotation) override;

    void set_field_of_view(float new_field_of_view);
    void set_aspect_ratio(float new_aspect_ratio);

protected:
    void on_attachment_to_tree() override final;

private:
    float field_of_view = glm::radians(90.0f);
    float aspect_ratio = 1920.0f / 1080.f;
    
    bool is_cached_view_matrix_valid = false;
    bool is_cached_proj_matrix_valid = false;
    glm::mat4 cached_view_matrix;
    glm::mat4 cached_proj_matrix;

    void recompute_view_matrix() noexcept;
    void recompute_proj_matrix() noexcept;
};
}