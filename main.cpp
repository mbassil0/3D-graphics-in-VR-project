#include<iostream>

//include glad before GLFW to avoid header conflict or define "#define GLFW_INCLUDE_NONE"
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>


#include<glm/gtc/matrix_inverse.hpp>

#include "stb_image.h"

#include "camera.h"
#include "shader.h"
#include "object.h"
#include "util.h"

#include "effects/explosion.h"
#include "effects/particle_system.h"

#include "effects/shadow.h"

#include "shaders.h"

#include "render.h"

//#define TINYOBJLOADER_IMPLEMENTATION
//#include "tiny_obj_loader.h"
//#include "tiny_gltf.h"

const int width = 500;
const int height = 500;

float prevx = 400.0f;
float prevy = 300.0f;



GLuint debugVAO;
GLuint debugVBO;





void handle_shooting(bool shoot, std::vector<Renderable> *renderables, glm::vec3 forward, ExplosionSystem *particleSystem)
{
	//put rocket launcher in the right position
	renderables->at(3).rotation = glm::vec3(-camera.Pitch, -camera.Yaw + 90.0f, 0.0f);
	
	//todo mettre dans updategamelogic
	//shoot is true if we are left clicking
	static bool has_been_shot = false;
	if (shoot)
	{
		has_been_shot = true;
		renderables->at(2).position = renderables->at(3).position; //set the rocket position to the rocket launcher
		renderables->at(2).rotation = renderables->at(3).rotation;
	}
	
	
	if (has_been_shot) {


		renderables->at(2).position += forward;
		if (renderables->at(2).position[1] < 0) { renderables->at(2).position[1] = 0; }
		renderables->at(2).rotation =
			glm::vec3(
				0.0f,
				-camera.Yaw + 90.0f,
				camera.Pitch
			);

		glm::vec3 car_bounds_min = renderables->at(1).min + renderables->at(1).position;
		glm::vec3 car_bounds_max = renderables->at(1).max + renderables->at(1).position;


		glm::vec3 rocket_position = renderables->at(2).position;
		if (
			car_bounds_min.x <= rocket_position.x &&
			car_bounds_max.x >= rocket_position.x &&

			car_bounds_min.y <= rocket_position.y &&
			car_bounds_max.y >= rocket_position.y &&

			car_bounds_min.z <= rocket_position.z &&
			car_bounds_max.z >= rocket_position.z
			)
		{

			particleSystem->emit(glm::vec3(renderables->at(2).position.x, renderables->at(2).position.y,renderables->at(2).position.z));
			renderables->at(1).destroyed = true;
		}
		else {
			
		}		
		
	}
}


void draw_direction_line(glm::vec3 position, float yaw, float pitch) {
	/*
	Draws the line of the
	For debugging/visualisation purposes
	*/
	glm::vec3 forward;

	forward.x =
		cos(glm::radians(yaw)) *
		cos(glm::radians(pitch));

	forward.y =
		sin(glm::radians(pitch));

	forward.z =
		sin(glm::radians(yaw)) *
		cos(glm::radians(pitch));

	forward = glm::normalize(forward);

	glm::vec3 start =
		position;

	glm::vec3 end =
		start + forward * 5.0f;

	float lineVertices[] =
	{
		start.x, start.y, start.z,
		end.x, end.y, end.z
	};
	glBindBuffer(GL_ARRAY_BUFFER, debugVBO);

	glBufferData(
		GL_ARRAY_BUFFER,
		sizeof(lineVertices),
		lineVertices,
		GL_DYNAMIC_DRAW
	);

	glDrawArrays(GL_LINES, 0, 2);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	static bool first = true;
	if (first)
	{
		prevx = xpos;
		prevy = ypos;
		first = false;
	}

	float xoffset = xpos - prevx;
	float yoffset = prevy - ypos;

	prevx = xpos;
	prevy = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}


void loadCubemapFace(const char* file, const GLenum& targetCube);







int main(int argc, char* argv[])
{

	GLFWwindow* window = Init();
	
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	

	Shader cubeMapShader = Shader(sourceVCubeMap, sourceFCubeMap);



	Shader lineShader(lineVS, lineFS);
	

	glGenVertexArrays(1, &debugVAO);
	glGenBuffers(1, &debugVBO);

	glBindVertexArray(debugVAO);

	glBindBuffer(GL_ARRAY_BUFFER, debugVBO);

	glBufferData(
		GL_ARRAY_BUFFER,
		sizeof(float) * 6,
		NULL,
		GL_DYNAMIC_DRAW
	);

	glVertexAttribPointer(
		0,
		3,
		GL_FLOAT,
		GL_FALSE,
		3 * sizeof(float),
		(void*)0
	);

	glEnableVertexAttribArray(0);


	RenderableMgr renderagbleMgr;

	
	renderagbleMgr.add_object("scene.obj", glm::vec3(-0.0, 0.0, -00.0), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.7, 0.7, 0.7)); //celuia la pompe!!!
	
	//renderagbleMgr.add_object("retro building.obj", glm::vec3(-4.0, 0.0, -10.0), glm::vec3(0.0, 0.0, 0.0), glm::vec3(1.5, 1.5, 1.5));
	renderagbleMgr.add_object("FINAL_MODEL_ASTERION.obj", glm::vec3(-1.5,0.0, 17.0), glm::vec3(0.0, 0.0, 0.0), glm::vec3(1.2, 1.2, 1.2)); 
	renderagbleMgr.add_object("rocket.obj", glm::vec3(-14.0, 1.0, 17.0), glm::vec3(0, 3.14 / 2, 0.0), glm::vec3(0.2, 0.2, 0.2));
	//renderagbleMgr.add_object("pool.obj", glm::vec3(-4.0, 0.0, -10.0), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.4, 0.4, 0.4));
	//renderagbleMgr.add_object("Untitled.obj", glm::vec3(-4.0, 0.0, -10.0), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.4, 0.4, 0.4));
	renderagbleMgr.add_object("rocket_launcher.obj", glm::vec3(-14.0, 1.0, 17.0), glm::vec3(0, 3.14 / 2, 0.0), glm::vec3(0.2, 0.2, 0.2));
	//renderagbleMgr.add_object("Street_Lamp_7.obj", glm::vec3(-0.0, 0.0, 0), glm::vec3(0, 3.14 / 2, 0.0), glm::vec3(4.0, 4.0, 4.0));
	renderagbleMgr.add_lamp("Street_Lamp_7.obj", glm::vec3(-17.5, 0.0, 12.5), glm::vec3(0, 3.14 / 2, 0.0), glm::vec3(4.0, 4.0, 4.0) , glm::vec3(0.0, 8.4, 0.0));
	renderagbleMgr.add_lamp("Street_Lamp_7.obj", glm::vec3(-17.5, 0.0, 23), glm::vec3(0, 3.14 / 2, 0.0), glm::vec3(4.0, 4.0, 4.0), glm::vec3(0.0, 8.4, 0.0));

	renderagbleMgr.add_lamp("Street_Lamp_7.obj", glm::vec3(17.5, 0.0, 12.5), glm::vec3(0, 3.14 / 2, 0.0), glm::vec3(4.0, 4.0, 4.0), glm::vec3(0.0, 8.4, 0.0));
	renderagbleMgr.add_lamp("Street_Lamp_7.obj", glm::vec3(17.5, 0.0, 23), glm::vec3(0, 3.14 / 2, 0.0), glm::vec3(4.0, 4.0, 4.0), glm::vec3(0.0, 8.4, 0.0));
	renderagbleMgr.add_lamp("Street_Lamp_7.obj", glm::vec3(-4.2, 0.0, -15), glm::vec3(0, 3.14 / 2, 0.0), glm::vec3(4.0, 4.0, 4.0), glm::vec3(0.0, 8.4, 0.0));
	Shader depthShader(depthVS, depthFS);
	ShadowMap shadowMap;
	shadowMap.Init();


	ExplosionSystem particleSystem;
	particleSystem.Init();
	double prev = 0;
	int deltaFrame = 0;
	//fps function
	auto fps = [&](double now) {
		double deltaTime = now - prev;
		deltaFrame++;
		if (deltaTime > 0.5) {
			prev = now;
			const double fpsCount = (double)deltaFrame / deltaTime;
			deltaFrame = 0;
			std::cout << "\r hFPS: " << fpsCount;
		}
	};


	glm::mat4 model = glm::mat4(1.0);
	model = glm::translate(model, glm::vec3(0.5, 0.5, -1.0));
	model = glm::scale(model, glm::vec3(1.0, 1.0, 1.0));
	
	

	glm::mat4 view = camera.GetViewMatrix();
	glm::mat4 perspective = camera.GetProjectionMatrix();


	char pathCube[] = "cube.obj";
	Object cubeMap(pathCube);
	cubeMap.makeObject(cubeMapShader);
	GLuint cubeMapTexture;
	glGenTextures(1, &cubeMapTexture);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMapTexture);

	// texture parameters
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	//stbi_set_flip_vertically_on_load(true);

	//std::string pathToCubeMap = "cubemaps/purple_sky/";
	std::string pathToCubeMap = "cubemaps/map/";
	std::map<std::string, GLenum> facesToLoad = {
		{pathToCubeMap + "posx.png",GL_TEXTURE_CUBE_MAP_POSITIVE_X},
		{pathToCubeMap + "posy.png",GL_TEXTURE_CUBE_MAP_POSITIVE_Y},
		{pathToCubeMap + "posz.png",GL_TEXTURE_CUBE_MAP_POSITIVE_Z},
		{pathToCubeMap + "negx.png",GL_TEXTURE_CUBE_MAP_NEGATIVE_X},
		{pathToCubeMap + "negy.png",GL_TEXTURE_CUBE_MAP_NEGATIVE_Y},
		{pathToCubeMap + "negz.png",GL_TEXTURE_CUBE_MAP_NEGATIVE_Z},
	};
	//load the six faces
	for (std::pair<std::string, GLenum> pair : facesToLoad) {
		loadCubemapFace(pair.first.c_str(), pair.second);

		printf("obtained %d \n", pair.second);
	}


	glfwSwapInterval(1);
	GLuint shaderV = compileShader(sourceVl, GL_VERTEX_SHADER);
	GLuint shaderF =compileShader(sourceFl, GL_FRAGMENT_SHADER);
	GLuint program = compileProgram(shaderV, shaderF);

	auto u_texture = glGetUniformLocation(program, "ourTexture");
	GLint useTextureLoc =glGetUniformLocation(program, "useTexture");
	GLint colorLoc = glGetUniformLocation(program, "materialColor");

	InputMgr inputmgr;

	while (!glfwWindowShouldClose(window))
	{
		
		
		
		UserCmd cur_cmd;
			
		processInput(window, &cur_cmd, &inputmgr);
		updateGameLogic(&renderagbleMgr.renderables.at(3), cur_cmd, &inputmgr);
		glm::vec3 forward;

		forward.x = cos(glm::radians(camera.Yaw)) * cos(glm::radians(camera.Pitch));
		forward.y = sin(glm::radians(camera.Pitch));
		forward.z = sin(glm::radians(camera.Yaw)) * cos(glm::radians(camera.Pitch));
		forward = glm::normalize(forward);
		//view = camera.GetViewMatrix();
		if (inputmgr.thirdPersonView)
		{
			glm::vec3 playerPos =	renderagbleMgr.renderables[3].position;

			// caméra derrière le joueur
			glm::vec3 camPos = playerPos - forward * 1.0f + glm::vec3(0.0f, 1.0f, 0.0f);
			view = glm::lookAt(camPos,camPos + forward,glm::vec3(0.0f, 2.0f, 0.0f));
		}
		else
		{
			view = camera.GetViewMatrix();
		}


		glfwPollEvents();

		double now = glfwGetTime();

		glClearColor(0.5f, 0.5f, 0.5f, 1.0f);

		glClear(
			GL_COLOR_BUFFER_BIT |
			GL_DEPTH_BUFFER_BIT
		);

		
		//light handling
		float t = glfwGetTime();
		float x =-4.0f + sin(t) * 2.0f;
		float y =7.50f ;
		std::vector<glm::vec3> lights;
		glm::vec3 light_pos =
			glm::vec3(x+4, y-1, -1.7f);
		/*  
		lights.push_back(
			glm::vec3(x + 4, y - 1, -1.7f)
		);

		lights.push_back(
			glm::vec3(-8.0f, 4.0f, 10.0f)
		);

		lights.push_back(
			glm::vec3(-3.0f, 15.0f, -19.0f)
		);
		lights.push_back(
			glm::vec3(12.0f, 8.0f, 19.0f)
		);*/
		
		printf("%f %f %f \n ", camera.Position[0], camera.Position[1], camera.Position[2]);
		for (int i = 0; i < renderagbleMgr.renderables.size(); i++) {
			if (renderagbleMgr.renderables[i].has_light) {
				lights.push_back(renderagbleMgr.renderables[i].light);
				//printf("pushing this light %f %f %f \n", renderagbleMgr.renderables[i].light[0], renderagbleMgr.renderables[i].light[1], renderagbleMgr.renderables[i].light[2]);
			}
		}

		draw_direction_line(	renderagbleMgr.renderables[2].position,	camera.Yaw,	camera.Pitch	);
		

		
		renderShadowPass( window, &renderagbleMgr.renderables,shadowMap, depthShader);
		RenderScene(window, &renderagbleMgr.renderables, shadowMap, view, perspective, program, colorLoc, useTextureLoc ,u_texture, lights);
		
		

		//todo mettre dans updategamelogic ?
		renderagbleMgr.renderables.at(2).rotation = renderagbleMgr.renderables.at(3).rotation;
		handle_shooting(cur_cmd.shoot, &renderagbleMgr.renderables, forward, &particleSystem);
		
		
		renderParticleSystem(&particleSystem, view, perspective);


		RenderSkybox(cubeMapShader, cubeMapTexture, cubeMap, view, perspective);
		lineShader.use();

		lineShader.setMatrix4("V", view);
		lineShader.setMatrix4("P", perspective);

		glBindVertexArray(debugVAO);

		draw_direction_line(		renderagbleMgr.renderables[2].position,	camera.Yaw,	camera.Pitch);
		glfwSwapBuffers(window);
	}
}



