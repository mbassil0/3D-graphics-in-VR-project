#pragma once
#include "camera.h"
#include "shader.h"
#include "object.h"
#include "util.h"

#include "effects/explosion.h"
#include "effects/particle_system.h"

#include "effects/shadow.h"


#include<glm/gtc/matrix_inverse.hpp>

void RenderScene(GLFWwindow* window, std::vector<Renderable>* renderables, ShadowMap shadowMap, glm::mat4 view, glm::mat4 perspective, GLuint program, GLint colorLoc, GLint useTextureLoc, GLint u_texture, std::vector<glm::vec3> light_pos);
void renderParticleSystem(ExplosionSystem* particleSystem, glm::mat4 view, glm::mat4 perspective);
void RenderSkybox(Shader cubeMapShader, GLuint cubeMapTexture, Object cubeMap, glm::mat4 view, glm::mat4 perspective);