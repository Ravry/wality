#include "renderer.hpp"

std::vector<float> vertices = {
    -1.f, -1.f, 0.0f,       0.f, 0.f,
     1.f, -1.f, 0.0f,       1.f, 0.f,
    -1.f,  1.f, 0.0f,       0.f, 1.f,
     1.f,  1.f, 0.0f,       1.f, 1.f
};

std::vector<unsigned int> indices = {
    0, 1, 2,
    1, 2, 3
};

std::unique_ptr<FBO> fbo;
std::unique_ptr<FBO> intermediateFBO;
std::unique_ptr<Shader> shader;
std::unique_ptr<Texture> texture;
std::unique_ptr<VAO> vao;
std::unique_ptr<VBO> vbo;
std::unique_ptr<EBO> ebo;
std::unique_ptr<Media> media;
std::unique_ptr<Texture> mediaTexture;

Renderer::Renderer()
{
    glClearColor(1.f, 1.f, 1.f, 1.f);

    std::cout << "creating fbo" << std::endl;

    // fbo = std::make_unique<FBO>(800, 600, GL_TEXTURE_2D_MULTISAMPLE);
    // intermediateFBO = std::make_unique<FBO>(800, 600, GL_TEXTURE_2D);

    m_width = 800;
    m_height = 600;

    std::cout << "loading shaders" << std::endl;

    shader = std::make_unique<Shader>("res/shaders/vertex.glsl", "res/shaders/fragment.glsl");
    texture = std::make_unique<Texture>("res/images/checkered.png", GL_TEXTURE_2D, GL_NEAREST);

    std::cout << "creating buffers" << std::endl;

    vao = std::make_unique<VAO>();
    vbo = std::make_unique<VBO>();
    ebo = std::make_unique<EBO>();

    vao->bind();
    vbo->bind();
    vbo->data(vertices);

    ebo->bind();
    ebo->data(indices);

    vao->attrib(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    vao->attrib(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

    vao->unbind();
    vbo->unbind();
    ebo->unbind();

    media = std::make_unique<Media>("res/images/video_2.mp4");
    mediaTexture = std::make_unique<Texture>(media.get());
}

int frame {0};
float duration {0.f};

void Renderer::render(float deltaTime)
{
    // Check and update texture only when a new frame is available
    if (!media->frameQueue.empty()) {
        mediaTexture->update(media->frameQueue.pop().value());    
    }

    // Single pass rendering
    glClear(GL_COLOR_BUFFER_BIT);

    // Use shader and texture once
    shader->use();
    mediaTexture->bind();
    vao->bind();

    // Single draw call
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

    // Unbind in reverse order
    vao->unbind();
    mediaTexture->unbind();
    shader->unuse();
}

void Renderer::refactor(unsigned int width, unsigned int height) {
    m_width = width;
    m_height = height;

    // fbo->refactor(width, height, GL_TEXTURE_2D_MULTISAMPLE);
    // intermediateFBO->refactor(width, height, GL_TEXTURE_2D);
}

void cleanup() {
    vao->destroy();
    vbo->destroy();
    ebo->destroy();
}