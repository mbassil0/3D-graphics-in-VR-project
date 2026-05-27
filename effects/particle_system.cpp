
#include <algorithm>
#include <chrono>
#include <thread>
#include <glm/gtc/matrix_inverse.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/norm.hpp>

#include "particle_system.h"

#include "../util.h"

const int MaxParticles = 1000000;
Particle particlesContainer[MaxParticles];

int lastUsedParticle = 0;

int findUnusedParticle(Particle *container) {
	for (int i = lastUsedParticle; i < MaxParticles; i++) {
		if (container[i].life < 0) {
			lastUsedParticle = i;
			return i;
		}
	}

	for (int i = 0; i < lastUsedParticle; i++) {
		if (container[i].life < 0) {
			lastUsedParticle = i;
			return i;
		}
	}

	return 0;
}

void sortParticles() {
	std::sort(&particlesContainer[0], &particlesContainer[MaxParticles]);
}

void ParticleSystem::Init()
{
	lastTime = glfwGetTime();
	

	if (!load_texture("smoke_05.png", &smokeTexture))
	{
		std::cout << "Failed to load smoke texture\n";
	}
	else
		std::cout << "loaded particle system texture " << smokeTexture << std::endl;

	// First object!
	const float vertexData[18] = {
		// vertices
		-1.0, -1.0, 0.0,
		1.0, -1.0, 0.0,
		-1.0, 1.0, 0.0,
		1.0, 1.0, 0.0,
		-1.0, 1.0, 0.0,
		1.0, -1.0, 0.0
	};


	g_particule_position_size_data = new GLfloat[MaxParticles * 4];
	g_particule_color_data = new GLfloat[MaxParticles * 4];

	for (int i = 0; i < MaxParticles; i++) {
		particlesContainer[i].life = -1.0;
	}


	
	glGenVertexArrays(1, &VAO1);
	glGenBuffers(1, &VBO_vertex);
	glGenBuffers(1, &VBO_position);
	glGenBuffers(1, &VBO_color);
	glBindVertexArray(VAO1);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_vertex);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData), vertexData, GL_STATIC_DRAW);

	auto att_vertex = glGetAttribLocation(shader.ID, "vertex");
	glEnableVertexAttribArray(att_vertex);
	glVertexAttribPointer(att_vertex, 3, GL_FLOAT, false, 0, 0);
	glVertexAttribDivisor(att_vertex, 0);


	glBindBuffer(GL_ARRAY_BUFFER, VBO_position);
	glBufferData(GL_ARRAY_BUFFER, MaxParticles * 4 * sizeof(GL_FLOAT), NULL, GL_STREAM_DRAW);

	auto att_center = glGetAttribLocation(shader.ID, "center");
	glEnableVertexAttribArray(att_center);
	glVertexAttribPointer(att_center, 4, GL_FLOAT, false, 0, 0);
	glVertexAttribDivisor(att_center, 1);


	glBindBuffer(GL_ARRAY_BUFFER, VBO_color);
	glBufferData(GL_ARRAY_BUFFER, MaxParticles * 4 * sizeof(GLfloat), NULL, GL_STREAM_DRAW);

	auto att_col = glGetAttribLocation(shader.ID, "col");
	glEnableVertexAttribArray(att_col);
	glVertexAttribPointer(att_col, 4, GL_FLOAT, true, 0, 0);
	glVertexAttribDivisor(att_col, 1);

	//desactive the buffer
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	
}


void ParticleSystem::Update(glm::vec3 cameraPosition)
{
	particleCount = 0;

	for (int i = 0; i < 10; i++)
	{
		int particleIndex = findUnusedParticle(particlesContainer);

		Particle& p = particlesContainer[particleIndex];

		p.life = 5.0f;

		p.pos = glm::vec3(-3.56f, 0.81f, -7.55f);

		p.speed = glm::vec3(
			((rand() % 100) - 50.0f) / 100.0f,
			1.0f,
			((rand() % 100) - 50.0f) / 100.0f
		);
		

		float heat = (rand() % 100) / 100.0f;

		glm::vec3 col;

		// HOT WHITE/YELLOW CENTER
		if (heat > 0.7f)
		{
			col = glm::vec3(
				1.0f,
				1.0f,
				0.8f
			);
		}
		// ORANGE
		else if (heat > 0.4f)
		{
			col = glm::vec3(
				1.0f,
				0.5f,
				0.0f
			);
		}
		// RED OUTER PARTICLES
		else
		{
			col = glm::vec3(
				0.8f,
				0.1f,
				0.0f
			);
		}

		p.color = glm::vec4(col, 0.35f);

		p.size = 0.4f;
	}


	double currentTime = glfwGetTime();
	double delta = currentTime - lastTime;
	lastTime = currentTime;

	for (int i = 0; i < MaxParticles; i++) {
		Particle& p = particlesContainer[i]; //shortcut

		if (p.life > 0.0) {
			//decrease life, use time since last frame
			p.life -= delta;

			//change of pos
			p.pos += p.speed * (float)delta;

			//p.color.y += (float)delta * 0.5;
			//p.color.r += glm::cos(rand()) * 0.5;


			p.color.a -= (float)delta * 0.05;

			//update distance with the camera
			p.cameraDist = glm::length2(p.pos - cameraPosition);

			//fill the gpu buffer
			g_particule_position_size_data[4 * particleCount] = p.pos.x;
			g_particule_position_size_data[4 * particleCount + 1] = p.pos.y;
			g_particule_position_size_data[4 * particleCount + 2] = p.pos.z;

			g_particule_position_size_data[4 * particleCount + 3] = p.size;

			g_particule_color_data[4 * particleCount + 0] = p.color.r;
			g_particule_color_data[4 * particleCount + 1] = p.color.g;
			g_particule_color_data[4 * particleCount + 2] = p.color.b;
			g_particule_color_data[4 * particleCount + 3] = p.color.a;

			particleCount++;
		}

		else {
			//make sure all dead particle will be put at the end of the list
			p.cameraDist = -1;
		}
		
	}
	sortParticles();
}

void ParticleSystem::Render()
{
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glDepthMask(GL_FALSE);


	glBindVertexArray(VAO1);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_position);
	glBufferData(GL_ARRAY_BUFFER, MaxParticles * 4 * sizeof(GL_FLOAT), NULL, GL_STREAM_DRAW);;
	glBufferSubData(GL_ARRAY_BUFFER, 0, particleCount * sizeof(GLfloat) * 4, g_particule_position_size_data);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_color);
	glBufferData(GL_ARRAY_BUFFER, MaxParticles * 4 * sizeof(GLfloat), NULL, GL_STREAM_DRAW);;
	glBufferSubData(GL_ARRAY_BUFFER, 0, particleCount * sizeof(GLfloat) * 4, g_particule_color_data);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, smokeTexture);

	shader.setInteger("smokeTex", 0);

	glDrawArraysInstanced(GL_TRIANGLES, 0, 6, particleCount);

	glDepthMask(GL_TRUE);
	glDisable(GL_BLEND);

}

 ParticleSystem::ParticleSystem() : shader("smoke.vert", "smoke.frag")
{

}