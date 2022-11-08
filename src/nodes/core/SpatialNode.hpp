#pragma once

#include <vector> // std::vector
#include <memory> // std::unique_ptr
#include <string> // std::string

#include <glm/vec3.hpp> // glm::vec3
#include <glm/gtx/quaternion.hpp> // glm::quat

class SpatialNode {
public:
    struct SpatialParams {
        glm::vec3 translation = {0.0f, 0.0f, 0.0f};
        glm::vec3 scale = {1.0f, 1.0f, 1.0f};
        glm::quat rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
    };

    std::string name;

    SpatialNode(const SpatialParams& spatial_params);
    virtual ~SpatialNode();

    void update_children();
    virtual void add_child(SpatialNode&& child);
    void remove_child(const size_t index);
    void remove_child(SpatialNode* const ptr);

    const std::vector<std::unique_ptr<SpatialNode>>& get_children();

    virtual void set_translation(const glm::vec3& new_trans);
    virtual void translate(const glm::vec3& translation);
    virtual void set_scale(const glm::vec3& new_scale);
    virtual void set_rotation(const glm::quat& new_rotation);

    const glm::vec3& get_translation() const noexcept;
    const glm::vec3& get_scale() const noexcept;
    const glm::quat& get_rotation() const noexcept;
    glm::mat4 get_local_matrix();
    glm::mat4 get_global_matrix();

    virtual void update();

protected:
    SpatialNode* parent = nullptr;

    /// The vector MUST NOT be changed outside the
    /// add_child and remove_child functions.
    std::vector<std::unique_ptr<SpatialNode>> children;

    glm::mat4 local_matrix;
    SpatialParams spatial_params;
    bool needs_recalculation = true;

    void recalculate_matrix();

    friend class SceneTree;
};
