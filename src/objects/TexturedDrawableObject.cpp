#include "../utils/utils.hpp"
#include "TexturedDrawableObject.hpp"

GLuint TexturedDrawableObject::program_id;
GLuint TexturedDrawableObject::mvp_matrix_uniform_id;
GLuint TexturedDrawableObject::model_matrix_uniform_id;
GLuint TexturedDrawableObject::normal_matrix_uniform_id;
GLuint TexturedDrawableObject::camera_direction_uniform_id;
GLuint TexturedDrawableObject::light_position_uniform_id;
std::array<PointLight::Uniforms, TX_DRW_POINT_LIGHTS_AMOUNT> TexturedDrawableObject::point_light_uniforms;

TexturedDrawableObject::TexturedDrawableObject(std::shared_ptr<Texture> texture,
    std::shared_ptr<Mesh> mesh) :
    texture(texture) {
    this->mesh = mesh;
}

TexturedDrawableObject::~TexturedDrawableObject() {

}

void TexturedDrawableObject::pre_init() {
    // Init shaders.
    program_id = utils::load_shaders("res/shaders/textured_vertex.glsl",
                                     "res/shaders/textured_fragment.glsl");
    // Init uniforms.
    mvp_matrix_uniform_id = glGetUniformLocation(program_id, "MVP");
    model_matrix_uniform_id = glGetUniformLocation(program_id, "MODEL_MATRIX");
    normal_matrix_uniform_id = glGetUniformLocation(program_id, "NORMAL_MATRIX");
    camera_direction_uniform_id = glGetUniformLocation(program_id, "CAMERA_DIRECTION");
    light_position_uniform_id = glGetUniformLocation(program_id, "LIGHT_POSITION");

    for (GLuint i = 0; i < TX_DRW_POINT_LIGHTS_AMOUNT; i++)
        point_light_uniforms[i] = PointLight::get_uniforms_id(program_id, "POINT_LIGHTS", i);
}

void TexturedDrawableObject::clean_up() {
    glDeleteProgram(program_id);
}

void TexturedDrawableObject::draw(const glm::mat4& vp) {
    // Vertices.
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, mesh->vertices_id);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

    // UVs.
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, mesh->uvs_id);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);

    // Normals.
    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, mesh->normals_id);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);

    // Bind textures.
    glBindTexture(GL_TEXTURE_2D, texture->get_id());

    // Uniforms.
    glm::mat4 model_matrix = compute_matrix();

    glm::mat4 mvp = vp * model_matrix;
    glUniformMatrix4fv(mvp_matrix_uniform_id, 1, GL_FALSE, &mvp[0][0]);

    glUniformMatrix4fv(model_matrix_uniform_id, 1, GL_FALSE, &model_matrix[0][0]);

    glm::mat4 normal_matrix = glm::transpose(glm::inverse(model_matrix));
    glUniformMatrix4fv(normal_matrix_uniform_id, 1, GL_FALSE, &normal_matrix[0][0]);
    
    glUniform3fv(camera_direction_uniform_id, 1, &(camera->direction[0]));

    for (GLuint i = 0; i < TX_DRW_POINT_LIGHTS_AMOUNT; i++)
        (*lights)[i].set_uniforms(point_light_uniforms[i]);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->indices_id);
    glDrawElements(GL_TRIANGLES, mesh->indices.size(), GL_UNSIGNED_SHORT, 0);

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
}
