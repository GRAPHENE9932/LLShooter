#include "PointLightNode.hpp" // PointLightNode
#include "RenderingServer.hpp" // RenderingServer

#include <glm/gtc/type_ptr.hpp> // glm::value_ptr

#include <string> // std::string

PointLightNode::PointLightNode(RenderingServer& rs, const Transform& transform) :
    CompleteSpatialNode(transform), rs(rs) {
    rs.register_point_light(this);
}

PointLightNode::~PointLightNode() {
    rs.unregister_point_light(this);
}

PointLightNode::Uniforms PointLightNode::get_uniforms_id(
    GLuint program_id, const std::string& var_name, GLuint index) {
    PointLightNode::Uniforms result;

    // Position.
    std::string position_string {var_name + '[' + std::to_string(index) + "].position"};
    result.position_id = glGetUniformLocation(program_id, position_string.c_str());

    // Color.
    std::string color_string {var_name + '[' + std::to_string(index) + "].color"};
    result.color_id = glGetUniformLocation(program_id, color_string.c_str());

    return result;
}

void PointLightNode::set_uniforms(const PointLightNode::Uniforms& uniforms) const {
    glUniform3fv(uniforms.position_id, 1, glm::value_ptr(get_translation()));
    glUniform3fv(uniforms.color_id, 1, glm::value_ptr(color));
}
