#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


#include "shadow.h"

void ShadowMap::Init()
    {
        glGenFramebuffers(1, &depthMapFBO);

        glGenTextures(1, &depthMap);

        glBindTexture(GL_TEXTURE_2D, depthMap);

        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_DEPTH_COMPONENT,
            SHADOW_WIDTH,
            SHADOW_HEIGHT,
            0,
            GL_DEPTH_COMPONENT,
            GL_FLOAT,
            NULL
        );

        glTexParameteri(
            GL_TEXTURE_2D,
            GL_TEXTURE_MIN_FILTER,
            GL_LINEAR
        );

        glTexParameteri(
            GL_TEXTURE_2D,
            GL_TEXTURE_MAG_FILTER,
            GL_LINEAR
        );

        glTexParameteri(
            GL_TEXTURE_2D,
            GL_TEXTURE_WRAP_S,
            GL_CLAMP_TO_BORDER
        );

        glTexParameteri(
            GL_TEXTURE_2D,
            GL_TEXTURE_WRAP_T,
            GL_CLAMP_TO_BORDER
        );

        float borderColor[] =
        {
            1.0f,
            1.0f,
            1.0f,
            1.0f
        };

        glTexParameterfv(
            GL_TEXTURE_2D,
            GL_TEXTURE_BORDER_COLOR,
            borderColor
        );

        glBindFramebuffer(
            GL_FRAMEBUFFER,
            depthMapFBO
        );

        glFramebufferTexture2D(
            GL_FRAMEBUFFER,
            GL_DEPTH_ATTACHMENT,
            GL_TEXTURE_2D,
            depthMap,
            0
        );

        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }




void renderShadowPass(GLFWwindow* window,  std::vector<Renderable>* renderables, ShadowMap shadowMap, Shader depthShader)
{
    // =========================
        // DEPTH PASS
        // =========================

    glm::mat4 lightProjection;
    glm::mat4 lightView;

    lightProjection = glm::ortho(
        -20.0f,
        20.0f,
        -20.0f,
        20.0f,
        1.0f,
        30.0f
    );

     //only handling from one light at the moment
    glm::vec3 light_pos = glm::vec3(-17.5, 8.4, 13.5);
    lightView = glm::lookAt(
        light_pos,
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 1.0f, 0.0f)
    );
    
        shadowMap.lightSpaceMatrix = lightProjection * lightView;

    glViewport(
        0,
        0,
        shadowMap.SHADOW_WIDTH,
        shadowMap.SHADOW_HEIGHT
    );

    glBindFramebuffer(
        GL_FRAMEBUFFER,
        shadowMap.depthMapFBO
    );

    glClear(GL_DEPTH_BUFFER_BIT);

    depthShader.use();

    depthShader.setMatrix4(
        "lightSpaceMatrix",
        shadowMap.lightSpaceMatrix
    );
    //printf("%f %f %f \n", camera.Position[0], camera.Position[1], camera.Position[2]);
    for (int i = 0;
        i < renderables->size();
        i++)
    {
        glm::mat4 model =
            glm::mat4(1.0f);

        model = glm::translate(
            model,
            renderables->at(i).position
        );

        model = glm::rotate(
            model,
            glm::radians(renderables->at(i).rotation.x),
            glm::vec3(1.0f, 0.0f, 0.0f)
        );

        model = glm::rotate(
            model,
            glm::radians(renderables->at(i).rotation.y),
            glm::vec3(0.0f, 1.0f, 0.0f)
        );

        model = glm::rotate(
            model,
            glm::radians(renderables->at(i).rotation.z),
            glm::vec3(0.0f, 0.0f, 1.0f)
        );

        model = glm::scale(
            model,
            renderables->at(i).scale
        );

        depthShader.setMatrix4(
            "M",
            model
        );

        glBindVertexArray(
            renderables->at(i).VAO
        );

        glDrawArrays(
            GL_TRIANGLES,
            0,
            renderables->at(i).vertexCount
        );
    }

    glBindFramebuffer(
        GL_FRAMEBUFFER,
        0
    );

    int display_w, display_h;

    glfwGetFramebufferSize(window, &display_w, &display_h);

    glViewport(0, 0, display_w, display_h);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

}