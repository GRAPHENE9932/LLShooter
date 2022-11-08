#pragma once

#include <set> // std::set
#include <type_traits> // std::true_type

#include "ColoredTextShader.hpp"
#include "SkyboxShader.hpp"
#include "CommonShader.hpp"

class RenderingServer;

class ShaderManager {
public:
    ShaderManager(RenderingServer& rs);

    void use_colored_text_shader(const glm::mat4& mvp, const glm::vec3& color);
    void use_skybox_shader(const glm::mat4& mvp);
    void use_common_shader(const Material& material, const glm::mat4& mvp_matrix,
        const glm::mat4& model_matrix);

    GLuint get_colored_text_program_id();
    GLuint get_skybox_program_id();
    GLuint get_common_program_id(const Material& material);

private:
    const CommonShader& get_common_shader(const Material& material);

    ColoredTextShader colored_text_shader;
    RenderingServer& rendering_server;
    SkyboxShader skybox_shader;

    struct ComShaderComparator {
        using is_transparent = std::true_type;

        bool operator()(const CommonShader& left, const CommonShader& right) const noexcept {
            return left.extract_parameters() < right.extract_parameters();
        }
        bool operator()(const CommonShader& left, const CommonShader::Parameters& right) const noexcept {
            return left.extract_parameters() < right;
        }
        bool operator()(const CommonShader::Parameters& left, const CommonShader& right) const noexcept {
            return left < right.extract_parameters();
        }
    };
    std::set<CommonShader, ComShaderComparator> common_shaders;
};
