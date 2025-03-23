#include "renderer.hpp"

std::vector<float> vertices = {
    -0.5f, -0.5f, 0.0f,
     0.5f, -0.5f, 0.0f,
     0.0f,  0.5f, 0.0f
};

std::vector<unsigned int> indices = {
    0, 1, 2
};

std::unique_ptr<Shader> shader;
std::unique_ptr<VAO> vao;
std::unique_ptr<VBO> vbo;
std::unique_ptr<EBO> ebo;

Renderer::Renderer()
{
    shader = std::make_unique<Shader>("shaders/vertex.glsl", "shaders/fragment.glsl");

    vao = std::make_unique<VAO>();
    vbo = std::make_unique<VBO>();
    ebo = std::make_unique<EBO>();

    vao->bind();
    vbo->bind();
    vbo->data(vertices);    

    ebo->bind();
    ebo->data(indices); 

    vao->attrib(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    vao->unbind();
    vbo->unbind();
    ebo->unbind();
}

void Renderer::render()
{
    // glClearColor(1.f, 1.f, 1.f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT);

    shader->use();
    vao->bind();
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    vao->unbind();
    shader->unuse();
}

void cleanup() {
    vao->destroy();
    vbo->destroy();
    ebo->destroy();
}