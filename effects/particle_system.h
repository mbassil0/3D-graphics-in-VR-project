#pragma once

#include <glm/glm.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>


#include "../shader.h"

struct Particle {

    glm::vec3 pos, speed;

    glm::vec4 color;

    float life;
    float maxLife;

    float size;

    float cameraDist;

    Particle()
        : pos(0.0f),
        speed(0.0f),
        color(1.0f),
        life(0.0f),
        maxLife(0.0f),
        size(0.0f),
        cameraDist(0.0f)
    {}

    bool operator<(const Particle& otherP) const {

        return this->cameraDist > otherP.cameraDist;
    }
};

class ParticleSystem {
public:
	ParticleSystem();
	
	void Init();
	void Render();
	void Update(glm::vec3 cameraPosition);
	Shader shader;
private:
	GLuint VAO1, VBO1;
	
	GLuint VBO_vertex, VBO_position, VBO_color;
	GLuint smokeTexture;
	int particleCount = 0;
	double lastTime;

	GLfloat* g_particule_position_size_data;
	GLfloat* g_particule_color_data;


};

int findUnusedParticle(Particle *container);
void sortParticles();