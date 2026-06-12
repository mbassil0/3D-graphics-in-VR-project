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

#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"

void RRenderTextt(
	GLuint fontTexture,
	stbtt_bakedchar* cdata,
	GLuint textVAO,
	GLuint textVBO,
	std::string text,
	float x,
	float y
)
{
	glBindVertexArray(textVAO);
	glBindBuffer(GL_ARRAY_BUFFER, textVBO);
	glBindTexture(GL_TEXTURE_2D, fontTexture);

	glBindVertexArray(textVAO);

	for (char c : text)
	{
		if (c < 32 || c >= 128)
			continue;

		stbtt_aligned_quad q;

		stbtt_GetBakedQuad(
			cdata,
			512,
			512,
			c - 32,
			&x,
			&y,
			&q,
			1
		);

		float vertices[] =
		{
			q.x0, q.y0, q.s0, q.t0,
			q.x1, q.y0, q.s1, q.t0,
			q.x1, q.y1, q.s1, q.t1,

			q.x0, q.y0, q.s0, q.t0,
			q.x1, q.y1, q.s1, q.t1,
			q.x0, q.y1, q.s0, q.t1
		};

		glBindBuffer(GL_ARRAY_BUFFER, textVBO);

		glBufferSubData(
			GL_ARRAY_BUFFER,
			0,
			sizeof(vertices),
			vertices
		);

		glDrawArrays(GL_TRIANGLES, 0, 6);
	}
}


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
	
	//renderagbleMgr.add_object("retro building.obj", glm::vec3(-4.0, -01.0, -10.0), glm::vec3(0.0, 0.0, 0.0), glm::vec3(1.5, 1.5, 1.5));
	renderagbleMgr.add_object("FINAL_MODEL_ASTERION.obj", glm::vec3(-1.5,0.0, 17.0), glm::vec3(0.0, 0.0, 0.0), glm::vec3(1.2, 1.2, 1.2)); 
	renderagbleMgr.add_object("rocket.obj", glm::vec3(24.0, 2.0, 17.0), glm::vec3(0, 3.14 / 2, 0.0), glm::vec3(0.2, 0.2, 0.2));
	renderagbleMgr.add_object("rocket_launcher.obj", glm::vec3(24.0, 2.0, 17.0), glm::vec3(0, 3.14 / 2, 0.0), glm::vec3(0.2, 0.2, 0.2));
	renderagbleMgr.add_object("sphere.obj", glm::vec3(45, 5.0, 14), glm::vec3(0, 3.14 / 2, 0.0), glm::vec3(3.0, 3.0, 3.0));
	Shader depthShader(depthVS, depthFS);
	//renderagbleMgr.add_object("pool.obj", glm::vec3(-4.0, 0.0, -10.0), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.4, 0.4, 0.4));
	//renderagbleMgr.add_object("Untitled.obj", glm::vec3(-4.0, 0.0, -10.0), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.4, 0.4, 0.4));
	
	//renderagbleMgr.add_object("Street_Lamp_7.obj", glm::vec3(-0.0, 0.0, 0), glm::vec3(0, 3.14 / 2, 0.0), glm::vec3(4.0, 4.0, 4.0));
	renderagbleMgr.add_lamp("Street_Lamp_7.obj", glm::vec3(-17.5, 0.0, 12.5), glm::vec3(0, 3.14 / 2, 0.0), glm::vec3(4.0, 4.0, 4.0) , glm::vec3(0.0, 8.4, 0.0));
	renderagbleMgr.add_lamp("Street_Lamp_7.obj", glm::vec3(-17.5, 0.0, 23), glm::vec3(0, 3.14 / 2, 0.0), glm::vec3(4.0, 4.0, 4.0), glm::vec3(0.0, 8.4, 0.0));

	renderagbleMgr.add_lamp("Street_Lamp_7.obj", glm::vec3(17.5, 0.0, 12.5), glm::vec3(0, 3.14 / 2, 0.0), glm::vec3(4.0, 4.0, 4.0), glm::vec3(0.0, 8.4, 0.0));
	renderagbleMgr.add_lamp("Street_Lamp_7.obj", glm::vec3(17.5, 0.0, 23), glm::vec3(0, 3.14 / 2, 0.0), glm::vec3(4.0, 4.0, 4.0), glm::vec3(0.0, 8.4, 0.0));

	//renderagbleMgr.add_lamp("cube_textured.obj", glm::vec3(14.5, 0.5, 14), glm::vec3(0, 3.14 / 2, 0.0), glm::vec3(2.0, 2.0, 2.0), glm::vec3(0.0, 8.4, 0.0));
	//renderagbleMgr.add_lamp("cube_textured.obj", glm::vec3(14.5, 4.5, 10), glm::vec3(0, 3.14 / 2, 0.0), glm::vec3(2.0, 2.0, 2.0), glm::vec3(0.0, 8.4, 0.0));
	//renderagbleMgr.add_lamp("Street_Lamp_7.obj", glm::vec3(-8.2, 0.0, 1), glm::vec3(0, 3.14 / 2, 0.0), glm::vec3(4.0, 4.0, 4.0), glm::vec3(0.0, 8.4, 0.0));


	
	ShadowMap shadowMap;
	shadowMap.Init();

	renderagbleMgr.renderables[1].reflectivity = 0.0f;
	renderagbleMgr.renderables[4].reflectivity = 0.95f;
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

	stbi_set_flip_vertically_on_load(false);

	std::string pathToCubeMap = "cubemaps/map/";
	//std::string pathToCubeMap = "cubemaps/map/";
	std::vector<std::pair<std::string, GLenum>> facesToLoad =
	{
		{pathToCubeMap + "posz.png", GL_TEXTURE_CUBE_MAP_POSITIVE_X},
		{pathToCubeMap + "negz.png", GL_TEXTURE_CUBE_MAP_NEGATIVE_X},

		{pathToCubeMap + "posx.png", GL_TEXTURE_CUBE_MAP_POSITIVE_Z},
		{pathToCubeMap + "negx.png", GL_TEXTURE_CUBE_MAP_NEGATIVE_Z},

		{pathToCubeMap + "posy.png", GL_TEXTURE_CUBE_MAP_POSITIVE_Y},
		{pathToCubeMap + "negy.png", GL_TEXTURE_CUBE_MAP_NEGATIVE_Y},
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








	Shader textShader(textVS, textFS);

	glm::mat4 textProjection =
		glm::ortho(
			0.0f,
			(float)width,
			(float)height,
			0.0f
		);

	GLuint fontTexture;

	stbtt_bakedchar cdata[96];

	unsigned char* ttf_buffer =
		new unsigned char[1 << 20];

	FILE* fontFile =
		fopen("cmmi10.ttf", "rb");

	fread(
		ttf_buffer,
		1,
		1 << 20,
		fontFile
	);

	fclose(fontFile);

	unsigned char* bitmap =
		new unsigned char[512 * 512];

	stbtt_BakeFontBitmap(
		ttf_buffer,
		0,
		32.0,
		bitmap,
		512,
		512,
		32,
		96,
		cdata
	);

	glGenTextures(1, &fontTexture);

	glBindTexture(
		GL_TEXTURE_2D,
		fontTexture
	);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	glTexImage2D(
		GL_TEXTURE_2D,
		0,
		GL_RED,
		512,
		512,
		0,
		GL_RED,
		GL_UNSIGNED_BYTE,
		bitmap
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

	GLuint textVAO;
	GLuint textVBO;

	glGenVertexArrays(1, &textVAO);
	glGenBuffers(1, &textVBO);

	glBindVertexArray(textVAO);

	glBindBuffer(GL_ARRAY_BUFFER, textVBO);

	glBufferData(
		GL_ARRAY_BUFFER,
		sizeof(float) * 6 * 4,
		NULL,
		GL_DYNAMIC_DRAW
	);

	glVertexAttribPointer(
		0,
		4,
		GL_FLOAT,
		GL_FALSE,
		4 * sizeof(float),
		(void*)0
	);

	glEnableVertexAttribArray(0);

	while (!glfwWindowShouldClose(window))
	{
		
		
		
		UserCmd cur_cmd;
			
		processInput(window, &cur_cmd, &inputmgr);
		renderagbleMgr.updateGameLogic(&renderagbleMgr.renderables.at(3), cur_cmd, &inputmgr);
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
		
		//printf("%f %f %f \n ", camera.Position[0], camera.Position[1], camera.Position[2]);

		//renderagbleMgr.getGround(camera.Position);
		for (int i = 0; i < renderagbleMgr.renderables.size(); i++) {
			if (renderagbleMgr.renderables[i].has_light) {
				lights.push_back(renderagbleMgr.renderables[i].light);
				//printf("pushing this light %f %f %f \n", renderagbleMgr.renderables[i].light[0], renderagbleMgr.renderables[i].light[1], renderagbleMgr.renderables[i].light[2]);
			}
		}

		draw_direction_line(	renderagbleMgr.renderables[2].position,	camera.Yaw,	camera.Pitch	);
		

		glDisable(GL_CULL_FACE);
		//shadowMap.renderShadowPass( window, &renderagbleMgr.renderables, depthShader, lights[0], camera.Position);
		RenderScene(window, &renderagbleMgr.renderables, cubeMapTexture, shadowMap, view, perspective, program, colorLoc, useTextureLoc ,u_texture, lights);
		
		

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

		glDisable(GL_DEPTH_TEST);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		textShader.use();
		textShader.use();

		glUniform1i(
			glGetUniformLocation(textShader.ID, "textTexture"),
			0
		);
		textShader.setMatrix4(
			"projection",
			textProjection
		);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		glDisable(GL_CULL_FACE);

		glActiveTexture(GL_TEXTURE0);

		glBindTexture(GL_TEXTURE_2D, fontTexture);

		glBindVertexArray(textVAO);
		glUniform3f(
			glGetUniformLocation(textShader.ID, "textColor"),
			1.0f,
			1.0f,
			1.0f
		);
		glBindBuffer(GL_ARRAY_BUFFER, textVBO);
		/* RRenderTextt(
			fontTexture,
			cdata,
			textVAO,
			textVBO,
			std::string("YOU WIN"),
			200,
			200
		);*/

		glEnable(GL_DEPTH_TEST);

		glDisable(GL_BLEND);

		glEnable(GL_CULL_FACE);

		glEnable(GL_DEPTH_TEST);
		glfwSwapBuffers(window);
	}


	delete[] ttf_buffer;
	delete[] bitmap;
}



