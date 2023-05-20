#include <fstream>
#include <sstream>
#include <memory>

#include <GL/glew.h>

#include "shader_loader.hpp"

using namespace llengine;

/// Loads and compiles vertex and fragment shaders from files,
/// compiles, links them and returns the program ID.
GLuint llengine::load_shaders(std::string_view vertex_shader_path,
                    std::string_view fragment_shader_path,
                    const std::vector<std::string>& defines) {
    using std::string_literals::operator""s;

    ManagedProgramID program_id = glCreateProgram();

    ManagedShaderID vertex_shader_id = load_vertex_shader(vertex_shader_path, defines);
    glAttachShader(program_id, vertex_shader_id);
    ManagedShaderID fragment_shader_id = load_fragment_shader(fragment_shader_path, defines);
    glAttachShader(program_id, fragment_shader_id);

    glLinkProgram(program_id);

    GLint result;
    glGetProgramiv(program_id, GL_LINK_STATUS, &result);
    if (result == GL_FALSE) {
        GLint info_log_length;
        glGetProgramiv(program_id, GL_INFO_LOG_LENGTH, &info_log_length);

        std::unique_ptr<char[]> error_message = std::make_unique<char[]>(info_log_length + 1);
        glGetProgramInfoLog(program_id, info_log_length, nullptr, error_message.get());

        throw std::runtime_error(
            "Failed to link the program.\n"
            "Path to the vertex shader file: "s + vertex_shader_path.data() + "\n"
            "Path to the fragment shader file: "s + fragment_shader_path.data() + "\n"
            "Error message: "s + error_message.get()
        );
    }

    glDetachShader(program_id, vertex_shader_id);
    glDetachShader(program_id, fragment_shader_id);

    return program_id.release();
}

/// Loads the vertex shader from file, compiles it and
/// returns the vertex shader ID.
ManagedShaderID llengine::load_vertex_shader(std::string_view vertex_shader_path,
                                   const std::vector<std::string>& defines) {
    using std::string_literals::operator""s;

    ManagedShaderID vertex_shader_id = glCreateShader(GL_VERTEX_SHADER);

    std::ifstream file_stream;
    file_stream.open(std::string(vertex_shader_path), std::ios::in);
    if (!file_stream) {
        throw std::runtime_error(
            "Failed to read the vertex shader. Path to the file: "s +
            vertex_shader_path.data()
        );
    }

    std::stringstream shader_code_ss;
    shader_code_ss << file_stream.rdbuf();
    std::string shader_code = shader_code_ss.str();

    // Insert defines.
    if (!defines.empty()) {
        // Find the position of the next line after "#version ...".
        const std::size_t version_pos {shader_code.find("#version")};
        if (version_pos == std::string::npos) {
            throw std::runtime_error(
                "Shader doesn't contains the \"#version\" directive.\n"
                "Path to the file: "s + vertex_shader_path.data()
            );
        }
        const std::size_t pos {shader_code.find('\n', version_pos) + 1};

        for (std::string_view cur_def : defines) {
            shader_code.insert(pos, "#define "s + cur_def.data() + '\n');
        }
    }

    const char* shader_code_c = shader_code.c_str();
    glShaderSource(vertex_shader_id, 1, &shader_code_c, nullptr);
    glCompileShader(vertex_shader_id);

    GLint result;
    glGetShaderiv(vertex_shader_id, GL_COMPILE_STATUS, &result);
    if (result == GL_FALSE) {
        GLint info_log_length;
        glGetShaderiv(vertex_shader_id, GL_INFO_LOG_LENGTH, &info_log_length);

        std::unique_ptr<char[]> error_message = std::make_unique<char[]>(info_log_length + 1);
        glGetShaderInfoLog(vertex_shader_id, info_log_length, nullptr, error_message.get());

        throw std::runtime_error(
            "Failed to compile the vertex shader.\n"
            "Path to the file: "s + vertex_shader_path.data() + "\n"
            "Error message: "s + error_message.get()
        );
    }

    return vertex_shader_id.release();
}

/// Loads the fragment shader from file, compiles it and
/// returns the fragment shader shader ID.
ManagedShaderID llengine::load_fragment_shader(std::string_view fragment_shader_path,
                                     const std::vector<std::string>& defines) {
    using std::string_literals::operator""s;

    ManagedShaderID fragment_shader_id = glCreateShader(GL_FRAGMENT_SHADER);

    std::ifstream file_stream;
    file_stream.open(std::string(fragment_shader_path), std::ios::in);
    if (!file_stream) {
        throw std::runtime_error(
            "Failed to read the fragment shader. Path to the file: "s +
            fragment_shader_path.data()
        );
    }

    std::stringstream shader_code_ss;
    shader_code_ss << file_stream.rdbuf();
    std::string shader_code = shader_code_ss.str();

    // Insert defines.
    if (!defines.empty()) {
        // Find the position of the next line after "#version ...".
        const std::size_t version_pos {shader_code.find("#version")};
        if (version_pos == std::string::npos) {
            throw std::runtime_error(
                "Shader doesn't contains the \"#version\" directive.\n"
                "Path to the file: "s + fragment_shader_path.data()
            );
        }
        const std::size_t pos {shader_code.find('\n', version_pos) + 1};

        for (std::string_view cur_def : defines) {
            shader_code.insert(pos, "#define "s + cur_def.data() + '\n');
        }
    }

    const char* shader_code_c = shader_code.c_str();
    glShaderSource(fragment_shader_id, 1, &shader_code_c, nullptr);
    glCompileShader(fragment_shader_id);

    GLint result;
    glGetShaderiv(fragment_shader_id, GL_COMPILE_STATUS, &result);
    if (result == GL_FALSE) {
        GLint info_log_length;
        glGetShaderiv(fragment_shader_id, GL_INFO_LOG_LENGTH, &info_log_length);

        std::unique_ptr<char[]> error_message = std::make_unique<char[]>(info_log_length + 1);
        glGetShaderInfoLog(fragment_shader_id, info_log_length, nullptr, error_message.get());

        throw std::runtime_error(
            "Failed to compile the fragment shader.\n"
            "Path to the file: "s + fragment_shader_path.data() + '\n' +
            "Error message: "s + error_message.get()
        );
    }

    return fragment_shader_id.release();
}
