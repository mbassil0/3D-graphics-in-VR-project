#pragma once
#include <glm/glm.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>


#include "../shader.h"

#include "particle_system.h"


class ExplosionSystem {
public:
	ExplosionSystem();

	void Init();
	void Render();
	void Update(glm::vec3 cameraPosition);
	void emit(glm::vec3 position);

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