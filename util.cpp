#include<iostream>
#include "util.h"






const int width = 900;
const int height = 900;


GLFWwindow* Init()
{

	if (!glfwInit()) {
		throw std::runtime_error("Failed to initialise GLFW \n");
	}
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifndef NDEBUG
	//create a debug context to help with Debugging
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);
#endif


	//Create the window
	GLFWwindow* window = glfwCreateWindow(width, height, "Solution 02", nullptr, nullptr);
	if (window == NULL)
	{
		glfwTerminate();
		throw std::runtime_error("Failed to create GLFW window\n");
	}

	glfwMakeContextCurrent(window);

	//load openGL function
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		throw std::runtime_error("Failed to initialize GLAD");
	}

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

#ifndef NDEBUG
	int flags;
	glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
	if (flags & GL_CONTEXT_FLAG_DEBUG_BIT)
	{
		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		//glDebugMessageCallback(glDebugOutput, nullptr);
		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
	}
#endif

	return window;
}
/*  
#define TINYGLTF_IMPLEMENTATION
#define TINYGLTF_NO_INCLUDE_RAPIDJSON
#define TINYGLTF_NO_INCLUDE_STB_IMAGE
#define TINYGLTF_NO_INCLUDE_STB_IMAGE_WRITE

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include "stb_image.h"
#include "stb_image_write.h"

#include "json.hpp"
#include "tiny_gltf.h"
*/
#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"
#include "tiny_gltf.h"
//#include "json.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"



void loadCubemapFace(const char* path, const GLenum& targetFace)
{
	int imWidth, imHeight, imNrChannels;
	unsigned char* data = stbi_load(path, &imWidth, &imHeight, &imNrChannels, 0);
	if (data)
	{

		glTexImage2D(targetFace, 0, GL_RGB, imWidth, imHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		//glGenerateMipmap(targetFace);
	}
	else {
		//std::cout << "Failed to Load texture " << path <<   std::endl;
		const char* reason = stbi_failure_reason();
		std::cout << (reason == NULL ? "Probably not implemented by the student" : reason) << std::endl;
	}
	stbi_image_free(data);
}


void load_gtlf(std::string path)
{
	/*tinygltf::Model model;
	tinygltf::TinyGLTF loader;

	std::string err, warn;

	bool success = loader.LoadBinaryFromFile(&model, &err, &warn, path);
	//std::cout
	if (!warn.empty()) std::cout << warn << std::endl;
	if (!err.empty()) std::cerr << err << std::endl;
	if (!success) {
		std::cerr << "Failed to load glTF\n";
		return;
	}*/

}



bool load_texture(std::string path, GLuint *texture) {
	//printf("loading %s \n", path.c_str());
	
	glGenTextures(1, texture);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, *texture);

	//3. Define the parameters for the texture
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	//4. Load the image
	stbi_set_flip_vertically_on_load(true);
	int imWidth, imHeight, imNrChannels;
	unsigned char* data = stbi_load(path.c_str(), &imWidth, &imHeight, &imNrChannels, 0);
	if (data)
	{
		GLenum format;
		if (imNrChannels == 1)
			format = GL_RED;
		else if (imNrChannels == 3)
			format = GL_RGB;
		else if (imNrChannels == 4)
			format = GL_RGBA;

		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imWidth, imHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glTexImage2D(GL_TEXTURE_2D,
			0,
			format,
			imWidth,
			imHeight,
			0,
			format,
			GL_UNSIGNED_BYTE,
			data);
		glGenerateMipmap(GL_TEXTURE_2D);
		stbi_image_free(data);
		return 1;
	}
	else {
		//std::cout << "Failed to Load texture " << path.c_str() << std::endl;
		const char* reason = stbi_failure_reason();
		//std::cout << reason << std::endl;
		
		stbi_image_free(data);
		return 0;
	}

}

bool RenderableMgr::add_object(std::string objName, glm::vec3 pos, glm::vec3 orientation, glm::vec3 scale)
{
	Renderable obj;
	obj.position = pos;
	obj.rotation = orientation;
	obj.scale = scale;
	if (load_obj(objName, &obj)) {
		// upload to OpenGL
		renderables.push_back(obj);
		return true;
	}
	else
	{
		printf("error loading object ");
		return false;
	}
}


bool RenderableMgr::add_lamp(std::string objName, glm::vec3 pos, glm::vec3 orientation, glm::vec3 scale, glm::vec3 light_pos) {

	Renderable obj;
	obj.position = pos;
	obj.rotation = orientation;
	obj.scale = scale;
	obj.has_light = true;
	obj.light = light_pos + pos;
	if (load_obj(objName, &obj)) {
		// upload to OpenGL
		renderables.push_back(obj);
		return true;
	}
	else
	{
		printf("error loading object ");
		return false;
	}
}
	



bool  RenderableMgr::load_obj(const std::string& path, Renderable* out) {
	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	std::vector<Vertexx> outVertices;
	std::string warn, err;


	bool success = tinyobj::LoadObj(
		&attrib, &shapes, &materials,
		&warn, &err,
		path.c_str()
	);

	

	for (int i = 0; i < shapes.size(); i++)
	{
		printf("shape %s \n", shapes.at(i).name.c_str());
	}
	
	for (int i = 0; i < materials.size(); i++) {
		
		std::string texName = materials[i].diffuse_texname;
		if (texName.empty())
			continue;

		printf("lLLLoading %s %s  \n ", materials.at(i).ambient_texname.c_str(), materials.at(i).diffuse_texname.c_str());

		if (textures.find(texName) == textures.end()) //check if the texture has already been loadded 
		{
			GLuint texID;
			//printf("calling load texture with %s \n", texName.c_str());
			if (load_texture(texName, &texID))
			{
				//printf("teeex %s %d \n", texName.c_str(), texID);
				//printf("loaded texture %s \n", texName.c_str());
				textures[texName] = texID;
			}

			else {
				printf("load texture failed: %s\n",texName.c_str());
				const char* reason =stbi_failure_reason();

				if (reason)
				{
					printf("stbi error: %s \n \n \n \n ",	reason);
				}
			}
		}
	}

	if (!warn.empty()) std::cout << "WARN: " << warn << std::endl;
	if (!err.empty()) std::cerr << "ERR: " << err << std::endl;
	if (!success) {
		std::cerr << "Failed to load OBJ\n";
		return false;
	}
	int ct = 0;
	// Extract vertices

	
		glm::vec3 min(999999.0f);
		glm::vec3 max(-999999.0f);

		int currentMaterial = -1;

		TextureRange currentRange;
		
		for (const auto& shape : shapes)
		{
			glm::vec3 curMin(999999.0f);;
			glm::vec3 curMax(-999999.0f);;
			

			for (size_t f = 0; f < shape.mesh.indices.size(); f += 3)
			{
				
				//handle textures
				int material_id =
					shape.mesh.material_ids[f / 3];

				// material changed
				if (material_id != currentMaterial)
				{
					// save previous range
					if (currentMaterial != -1)
					{
						out->material.textureRanges.push_back(
							currentRange
						);
					}


					currentMaterial = material_id;
					tinyobj::material_t& mat =
						materials[material_id];

					currentRange.startVertex =
						outVertices.size();

					currentRange.vertexCount = 0;

					currentRange.texture.id = 0;

					// get texture from material
					if (material_id >= 0)
					{
						std::string texName =materials[material_id].diffuse_texname;
						//if ((texName != "Background_baseColor.png"))
							//continue;
						/*std::cout << "tex " << texName << " "
							<< (texName == "Background_baseColor.png")
							<< std::endl; */
						if (!texName.empty())
						{
							auto it = textures.find(texName);
							
							if (it == textures.end())
							{
								printf("MISSING TEXTURE: %s\n",
									texName.c_str());

								currentRange.texture.id = 0;
							}
							else
							{
								currentRange.hasTexture = true;
								currentRange.texture.id =
									it->second;

								printf("USING TEXTURE %s id=%d\n",
									texName.c_str(),
									it->second);
							}
						}
						else {
							
							currentRange.hasTexture = false;
							
							currentRange.color = glm::vec4(
								mat.diffuse[0],
								mat.diffuse[1],
								mat.diffuse[2],
								mat.dissolve
							);
						}
					}
				}

				// process the 3 vertices of the face
				for (int k = 0; k < 3; k++)
				{
					tinyobj::index_t index =
						shape.mesh.indices[f + k];

					Vertexx v{};

					// position
					v.px = attrib.vertices[
						3 * index.vertex_index + 0];

					v.py = attrib.vertices[
						3 * index.vertex_index + 1];

					v.pz = attrib.vertices[
						3 * index.vertex_index + 2];

					// normal
					if (!attrib.normals.empty()
						&& index.normal_index >= 0)
					{
						v.nx = attrib.normals[
							3 * index.normal_index + 0];

						v.ny = attrib.normals[
							3 * index.normal_index + 1];

						v.nz = attrib.normals[
							3 * index.normal_index + 2];
					}

					// uv
					if (!attrib.texcoords.empty()
						&& index.texcoord_index >= 0)
					{
						v.u = attrib.texcoords[
							2 * index.texcoord_index + 0];

						v.v = attrib.texcoords[
							2 * index.texcoord_index + 1];


						//printf("u v %f %f \n", v.u, v.v);
					}

					
					float x = attrib.vertices[3 * index.vertex_index + 0];
					float y = attrib.vertices[3 * index.vertex_index + 1];
					float z = attrib.vertices[3 * index.vertex_index + 2];

					curMin.x = std::min(curMin.x, x);
					curMin.y = std::min(curMin.y, y);
					curMin.z = std::min(curMin.z, z);

					curMax.x = std::max(curMax.x, x);
					curMax.y = std::max(curMax.y, y);
					curMax.z = std::max(curMax.z, z);

					outVertices.push_back(v);

					currentRange.vertexCount++;
				}
			}

			printf("okaok   %d  %s \n", path == "rocket_launcher.obj", path.c_str());
			if (path != "rocket_launcher.obj") {
				Shape curShape;

				curShape.min = curMin;
				curShape.max = curMax;
				curShape.renderableIndex = renderables.size(); //no need to -1 because we push_back after this line

				sceneShapes.push_back(curShape);
				printf("for object min %f %f %f   %f  %f  %f \n", curMin.x, curMin.y, curMin.z, curMax.x, curMax.y, curMax.z);
			}
		}

		// push the final range
		if (currentRange.vertexCount > 0)
		{
			out->material.textureRanges.push_back(
				currentRange
			);
		}



		std::cout << "Loaded OBJ: " << path << std::endl;
		std::cout << "Vertex count: " << outVertices.size() << std::endl;
		out->vertexCount = outVertices.size();

	
		for (auto& v : outVertices)
		{
			min.x = std::min(min.x, v.px);
			min.y = std::min(min.y, v.py);
			min.z = std::min(min.z, v.pz);

			max.x = std::max(max.x, v.px);
			max.y = std::max(max.y, v.py);
			max.z = std::max(max.z, v.pz);
		}

		out->min = min;
		out->max = max;

	glGenVertexArrays(1, &out->VAO);
	glGenBuffers(1, &out->VBO);

	glBindVertexArray(out->VAO);

	glBindBuffer(GL_ARRAY_BUFFER, out->VBO);
	glBufferData(GL_ARRAY_BUFFER,
		outVertices.size() * sizeof(Vertexx),
		outVertices.data(),
		GL_STATIC_DRAW);

	// position
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertexx), (void*)0);
	glEnableVertexAttribArray(0);

	// normal
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertexx), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	// uv 
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertexx), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	glBindVertexArray(0);
	return true;

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
