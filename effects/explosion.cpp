#include <algorithm>
#include <chrono>
#include <thread>

#include <glm/gtc/matrix_inverse.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/norm.hpp>

#include "explosion.h"

#include "../util.h"

const int MaxParticlesEx = 1000000;

Particle particlesContainere[MaxParticlesEx];

int lastUsedParticlee = 0;

void ExplosionSystem::Init()
{
    lastTime = glfwGetTime();

    if (!load_texture("smoke_05.png", &smokeTexture))
    {
        std::cout << "Failed to load smoke texture\n";
    }

    const float vertexData[18] = {

        -1.0f, -1.0f, 0.0f,
         1.0f, -1.0f, 0.0f,
        -1.0f,  1.0f, 0.0f,

         1.0f,  1.0f, 0.0f,
        -1.0f,  1.0f, 0.0f,
         1.0f, -1.0f, 0.0f
    };

    g_particule_position_size_data =
        new GLfloat[MaxParticlesEx * 4];

    g_particule_color_data =
        new GLfloat[MaxParticlesEx * 4];

    for (int i = 0; i < MaxParticlesEx; i++)
    {
        particlesContainere[i].life = -1.0f;
    }

    glGenVertexArrays(1, &VAO1);

    glGenBuffers(1, &VBO_vertex);
    glGenBuffers(1, &VBO_position);
    glGenBuffers(1, &VBO_color);

    glBindVertexArray(VAO1);

    // QUAD

    glBindBuffer(GL_ARRAY_BUFFER, VBO_vertex);

    glBufferData(
        GL_ARRAY_BUFFER,
        sizeof(vertexData),
        vertexData,
        GL_STATIC_DRAW
    );

    GLint att_vertex =
        glGetAttribLocation(shader.ID, "vertex");

    glEnableVertexAttribArray(att_vertex);

    glVertexAttribPointer(
        att_vertex,
        3,
        GL_FLOAT,
        GL_FALSE,
        0,
        0
    );

    glVertexAttribDivisor(att_vertex, 0);

    // POSITION

    glBindBuffer(GL_ARRAY_BUFFER, VBO_position);

    glBufferData(
        GL_ARRAY_BUFFER,
        MaxParticlesEx * 4 * sizeof(GLfloat),
        NULL,
        GL_STREAM_DRAW
    );

    GLint att_center =
        glGetAttribLocation(shader.ID, "center");

    glEnableVertexAttribArray(att_center);

    glVertexAttribPointer(
        att_center,
        4,
        GL_FLOAT,
        GL_FALSE,
        0,
        0
    );

    glVertexAttribDivisor(att_center, 1);

    // COLOR

    glBindBuffer(GL_ARRAY_BUFFER, VBO_color);

    glBufferData(
        GL_ARRAY_BUFFER,
        MaxParticlesEx * 4 * sizeof(GLfloat),
        NULL,
        GL_STREAM_DRAW
    );

    GLint att_col =
        glGetAttribLocation(shader.ID, "col");

    glEnableVertexAttribArray(att_col);

    glVertexAttribPointer(
        att_col,
        4,
        GL_FLOAT,
        GL_FALSE,
        0,
        0
    );

    glVertexAttribDivisor(att_col, 1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindVertexArray(0);
}

void ExplosionSystem::emit(glm::vec3 position)
{
    for (int i = 0; i < 6000; i++)
    {
        int particleIndex =
            findUnusedParticle(
                particlesContainere
            );

        Particle& p =
            particlesContainere[
                particleIndex
            ];

        p.life = 1.5f;
        p.maxLife = p.life;

        p.pos = position;

        glm::vec3 dir =
            glm::normalize(glm::vec3(

                ((rand() % 80) - 40.0f),

                ((rand() % 200) - 50.0f),

                ((rand() % 80) - 50.0f)
            ));

        float speed =
            0.00001f +
            (rand() % 100) / 8.0f;

        p.speed = dir * speed;

        p.size =
            0.4f +
            (rand() % 100) / 4000.0f;

        p.color =
            glm::vec4(1.0f);
    }
}

void ExplosionSystem::Update(glm::vec3 cameraPosition)
{
    particleCount = 0;

    double currentTime = glfwGetTime();

    double delta =
        currentTime - lastTime;

    lastTime = currentTime;

  

   
    // UPDATE PARTICLES

    for (int i = 0; i < MaxParticlesEx; i++)
    {
        Particle& p =
            particlesContainere[i];

        if (p.life > 0.0f)
        {
            p.life -= delta;

            p.pos +=
                p.speed * (float)delta;

            // DRAG

            p.speed *= 0.920f;

            // UPWARD FORCE

            p.speed.y +=
                0.00003f * delta;

            // SMALL EXPANSION

            p.size +=
                delta * 0.00003f;

            // SLOW FADE

            p.color.a *=
                0.995f;

            // LIFE STAGE

            float t =
                p.life / p.maxLife;

            // WHITE HOT

            if (t > 0.7f)
            {
                p.color.r = 1.0f;
                p.color.g = 0.95f;
                p.color.b = 0.8f;
                p.color.a = 0.9f;
            }

            // ORANGE

            else if (t > 0.4f)
            {
                p.color.r = 1.0f;
                p.color.g = 0.45f;
                p.color.b = 0.05f;
                p.color.a = 0.5f;
            }

            // SMOKE

            else
            {
                p.color.r = 0.2f;
                p.color.g = 0.2f;
                p.color.b = 0.2f;
                p.color.a = 0.2f;
            }

            p.cameraDist =
                glm::length2(
                    p.pos - cameraPosition
                );

            // POSITION

            g_particule_position_size_data[
                4 * particleCount + 0
            ] = p.pos.x;

            g_particule_position_size_data[
                4 * particleCount + 1
            ] = p.pos.y;

            g_particule_position_size_data[
                4 * particleCount + 2
            ] = p.pos.z;

            g_particule_position_size_data[
                4 * particleCount + 3
            ] = p.size;

            // COLOR

            g_particule_color_data[
                4 * particleCount + 0
            ] = p.color.r;

            g_particule_color_data[
                4 * particleCount + 1
            ] = p.color.g;

            g_particule_color_data[
                4 * particleCount + 2
            ] = p.color.b;

            g_particule_color_data[
                4 * particleCount + 3
            ] = p.color.a;

            particleCount++;
        }
        else
        {
            p.cameraDist = -1.0f;
        }
    }


    sortParticles();
}

void ExplosionSystem::Render()
{
    //std::cout << particleCount <<  "  "  << shader.ID  << std::endl;
    
    shader.use();

    glEnable(GL_BLEND);

    // ADDITIVE BLENDING

    glBlendFunc(
        GL_SRC_ALPHA,
        GL_ONE
    );

    glDepthMask(GL_FALSE);

    glBindVertexArray(VAO1);

    // POSITION BUFFER

    glBindBuffer(
        GL_ARRAY_BUFFER,
        VBO_position
    );

    glBufferData(
        GL_ARRAY_BUFFER,
        MaxParticlesEx * 4 * sizeof(GLfloat),
        NULL,
        GL_STREAM_DRAW
    );

    glBufferSubData(
        GL_ARRAY_BUFFER,
        0,
        particleCount *
        sizeof(GLfloat) * 4,
        g_particule_position_size_data
    );

    // COLOR BUFFER

    glBindBuffer(
        GL_ARRAY_BUFFER,
        VBO_color
    );

    glBufferData(
        GL_ARRAY_BUFFER,
        MaxParticlesEx * 4 * sizeof(GLfloat),
        NULL,
        GL_STREAM_DRAW
    );

    glBufferSubData(
        GL_ARRAY_BUFFER,
        0,
        particleCount *
        sizeof(GLfloat) * 4,
        g_particule_color_data
    );

    // TEXTURE

    glActiveTexture(GL_TEXTURE0);

    glBindTexture(
        GL_TEXTURE_2D,
        smokeTexture
    );

    shader.setInteger(
        "smokeTex",
        0
    );

    glDrawArraysInstanced(
        GL_TRIANGLES,
        0,
        6,
        particleCount
    );

    glDepthMask(GL_TRUE);

    glDisable(GL_BLEND);
}

ExplosionSystem::ExplosionSystem()
    : shader("smoke.vert", "smoke.frag")
{
}