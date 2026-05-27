#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "camera.h"

#include <vector>
#include <unordered_map>
#include <string>

#include "stb_image.h"


#include "tiny_obj_loader.h"
#include "tiny_gltf.h"



extern Camera camera;


struct Vertexx {
	float px, py, pz;
	float nx, ny, nz;
	float u, v;
};

/*  
struct Mesh {
	GLuint VAO;
	GLuint VBO;
	size_t vertexCount;
};*/

class Texture {
public:
	GLuint id;
	std::string type;
};

struct TextureRange
{
	Texture texture;

	int startVertex;

	int vertexCount;
	bool hasTexture = false;
	glm::vec4 color;
};
class Material {
public:
	std::vector<TextureRange> textureRanges;

};

struct Renderable {
	GLuint VAO;
	GLuint VBO;
	Material material;

	size_t vertexCount;
	glm::vec3 position;
	glm::vec3 rotation;
	glm::vec3 scale;
	

	glm::vec3 min;
	glm::vec3 max;
	bool destroyed = false; //for car
	bool has_light = false;
	glm::vec3 light; //possible light position on the obje
};

class RenderableMgr {
public:
	bool add_object(std::string objName, glm::vec3 pos, glm::vec3 orientation, glm::vec3 scale);
	bool add_lamp(std::string objName, glm::vec3 pos, glm::vec3 orientation, glm::vec3 scale, glm::vec3 light_pos);
	bool  load_obj(const std::string& path, Renderable* out);

	bool  load_obj2(const std::string& path, Renderable* out);

	void render();

	std::vector <Renderable> renderables;
	std::unordered_map<std::string, GLuint> textures;
	
	std::vector<glm::vec3> lights;

};

struct UserCmd
{
	bool left=0;
	bool right=0;
	bool forward=0;
	bool backward=0;

	bool jump=0;
	bool shoot=0;

	float yaw;
	float pitch;
};

struct InputMgr
{
	bool thirdPersonView = false;
	bool tabPressedLastFrame = false;


};

/*  
struct Renderable {
	//float x, y, z;
	glm::vec3 position;
	glm::vec3 rotation;
	glm::vec3 scale;

	//std::vector<Vertexx> vertices;
	GLuint VAO, VBO;

};*/









void load_gtlf(std::string path);
//bool load_obj(const std::string& path, std::vector<Vertexx>& outVertices);


//bool  load_obj(const std::string& path, std::vector<Vertexx>& outVertices, GLuint *VAO, GLuint *VBO);



bool load_texture(std::string path, GLuint* texture);



void processInput(GLFWwindow* window, UserCmd *cmd, InputMgr *input_mgr);
GLFWwindow* Init();
//void APIENTRY glDebugOutput(GLenum source,GLenum type,unsigned int id,GLenum severity,GLsizei length,const char* message,	const void* userParam);
void loadCubemapFace(const char* path, const GLenum& targetFace);


void updateGameLogic(Renderable* renderable, UserCmd cmd, InputMgr* out);