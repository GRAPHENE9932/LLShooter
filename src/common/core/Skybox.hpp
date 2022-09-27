#pragma once

#include <memory> // std::shared_ptr

#include "common/core/Texture.hpp" // Texture
#include "Context.hpp" // Context

class SceneTree;

class Skybox {
public:
    SceneTree& scene_tree;
    std::shared_ptr<Texture> texture;

    Skybox(SceneTree& scene_tree, 
           const std::shared_ptr<Texture>& texture) :
           scene_tree(scene_tree), texture(texture) {};

    static void static_init();
    static void static_clean_up();

    void draw();

private:
    static GLuint vertices_id;
};
