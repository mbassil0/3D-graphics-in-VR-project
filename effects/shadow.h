#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include "../shader.h"
#include "../util.h"

class ShadowMap {
public:

    GLuint depthMapFBO;
    GLuint depthMap;

    const unsigned int SHADOW_WIDTH = 2048;
    const unsigned int SHADOW_HEIGHT = 2048;

    glm::mat4 lightSpaceMatrix;

    void Init();
};

void renderShadowPass(GLFWwindow* window, std::vector<Renderable>* renderables, ShadowMap shadowMap, Shader depthShader);