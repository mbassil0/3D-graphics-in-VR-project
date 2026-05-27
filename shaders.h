// shaders.h
#pragma once
#include <string>
#include "shader.h"

const std::string depthVS =
"#version 330 core\n"

"layout (location = 0) in vec3 position;\n"

"uniform mat4 M;\n"
"uniform mat4 lightSpaceMatrix;\n"

"void main()\n"
"{\n"
"    gl_Position = lightSpaceMatrix * M * vec4(position, 1.0);\n"
"}\n";

const std::string depthFS =
"#version 330 core\n"

"void main()\n"
"{\n"
"}\n";

const std::string lineVS =
"#version 330 core\n"
"layout(location = 0) in vec3 position;\n"

"uniform mat4 V;\n"
"uniform mat4 P;\n"

"void main()\n"
"{\n"
"   gl_Position = P * V * vec4(position,1.0);\n"
"}\n";

const std::string lineFS =
"#version 330 core\n"

"out vec4 FragColor;\n"

"void main()\n"
"{\n"
"   FragColor = vec4(1,0,0,1);\n"
"}\n";

// // green terrain\n" yavait ca avant fragcolor
const std::string sourceVCubeMap = "#version 330 core\n"
"in vec3 position; \n"
"in vec2 tex_coords; \n"
"in vec3 normal; \n"

//only P and V are necessary
"uniform mat4 V; \n"
"uniform mat4 P; \n"

"out vec3 texCoord_v; \n"

" void main(){ \n"
"texCoord_v = position;\n"
//remove translation info from view matrix to only keep rotation
"mat4 V_no_rot = mat4(mat3(V)) ;\n"
"vec4 pos = P * V_no_rot * vec4(position, 1.0); \n"
// the positions xyz are divided by w after the vertex shader
// the z component is equal to the depth value
// we want a z always equal to 1.0 here, so we set z = w!
// Remember: z=1.0 is the MAXIMUM depth value ;)
"gl_Position = pos.xyww;\n"
"\n"
"}\n";

const std::string sourceFCubeMap =
"#version 330 core\n"
"out vec4 FragColor;\n"
"precision mediump float; \n"
"uniform samplerCube cubemapSampler; \n"
"in vec3 texCoord_v; \n"
"void main() { \n"
"FragColor = texture(cubemapSampler,texCoord_v); \n"
"} \n";


const std::string sourceVTex = "#version 330 core\n"
"in vec3 position; \n"
//5. Your code
"in vec2 texcoord; \n"
"out vec2 v_tex; \n"

"uniform mat4 M; \n"
"uniform mat4 V; \n"
"uniform mat4 P; \n"
" void main(){ \n"
"gl_Position = P*V*M*vec4(position, 1);\n"
"v_tex = texcoord; \n"

"}\n";
const std::string sourceFTex = "#version 330 core\n"
"out vec4 FragColor;"
"precision mediump float; \n"
"in vec2 v_tex; \n"
//6. Get the texture 
"uniform sampler2D ourTexture; \n"
"void main() { \n"
//8. use the texture and the coordinates
"FragColor = texture(ourTexture, v_tex); \n"
"} \n";


const std::string sourceVV = "#version 330 core\n"
"layout (location = 0) in vec3 position; \n"
"layout (location = 1) in vec3 normal; \n"
"layout (location = 2) in vec2 texcoord; \n"
"out vec2 v_tex; \n"
"uniform mat4 M; \n"
"uniform mat4 V; \n"
"uniform mat4 P; \n"
"void main(){ \n"
"gl_Position = P*V*M*vec4(position, 1.0);\n"
"v_tex = texcoord; \n"
"}\n";


const std::string sourceFF = "#version 330 core\n"
"out vec4 FragColor;\n"
"precision mediump float; \n"

"in vec2 v_tex; \n"

"uniform sampler2D ourTexture; \n"
"uniform bool useTexture; \n"
"uniform vec4 materialColor; \n"

"void main() { \n"

"	if(useTexture) \n"
"	{ \n"
"		vec4 tex = texture(ourTexture, v_tex); \n"

"		if(tex.a < 0.01) discard; \n"

"		FragColor = tex; \n"
"	} \n"
"	else \n"
"	{ \n"
"		FragColor = materialColor; \n"
"	} \n"

"} \n";


const std::string sourceVl = "#version 330 core\n"
"layout (location = 0) in vec3 position; \n"
"layout (location = 1) in vec3 normal; \n"
"layout (location = 2) in vec2 texcoord; \n"

"out vec3 v_frag_pos; \n"
"out vec3 v_normal; \n"
"out vec2 v_tex; \n"
"out vec4 fragPosLightSpace; \n"

"uniform mat4 M; \n"
"uniform mat4 itM; \n"
"uniform mat4 V; \n"
"uniform mat4 P; \n"
"uniform mat4 lightSpaceMatrix; \n"

"void main(){ \n"

"    vec4 world_pos = M * vec4(position, 1.0); \n"

"    gl_Position = P * V * world_pos; \n"

"    v_frag_pos = world_pos.xyz; \n"

"    fragPosLightSpace = lightSpaceMatrix * world_pos; \n"

"    v_normal = normalize(vec3(itM * vec4(normal, 0.0))); \n"

"    v_tex = texcoord; \n"

"}\n";


const std::string sourceFl = "#version 330 core\n"

"out vec4 FragColor;\n"

"in vec3 v_frag_pos; \n"
"in vec3 v_normal; \n"
"in vec2 v_tex; \n"
"in vec4 fragPosLightSpace; \n"

"uniform sampler2D ourTexture; \n"
"uniform sampler2D shadowMap; \n"

"uniform bool useTexture; \n"

"uniform vec4 materialColor; \n"

"uniform vec3 lightPositions[10]; \n"
"uniform vec3 lightColor; \n"

"uniform int numLights; \n"

"uniform vec3 viewPos; \n"

"float ShadowCalculation(vec4 fragPosLightSpace, vec3 N, vec3 L)\n"
"{\n"

"    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;\n"

"    projCoords = projCoords * 0.5 + 0.5;\n"

"    if(projCoords.x < 0.0 || projCoords.x > 1.0 ||\n"
"       projCoords.y < 0.0 || projCoords.y > 1.0)\n"
"    {\n"
"        return 0.0;\n"
"    }\n"

"    if(projCoords.z > 1.0)\n"
"    {\n"
"        return 0.0;\n"
"    }\n"

"    float currentDepth = projCoords.z;\n"

"    float bias = max(0.005 * (1.0 - dot(N, L)), 0.002);\n"

"    float shadow = 0.0;\n"

"    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);\n"

"    for(int x = -1; x <= 1; ++x)\n"
"    {\n"

"        for(int y = -1; y <= 1; ++y)\n"
"        {\n"

"            float pcfDepth = texture(\n"
"                shadowMap,\n"
"                projCoords.xy + vec2(x, y) * texelSize\n"
"            ).r;\n"

"            shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;\n"

"        }\n"

"    }\n"

"    shadow /= 9.0;\n"

"    return shadow;\n"

"}\n"

"void main() { \n"

"    vec3 N = normalize(v_normal); \n"

"    vec3 V = normalize(viewPos - v_frag_pos); \n"

"    vec3 diffuse = vec3(0.0);\n"
"    vec3 specular = vec3(0.0);\n"

"    vec3 firstLightDir = normalize(lightPositions[0] - v_frag_pos);\n"

"    float intensity = 2.0;\n"

"    for(int i = 0; i < numLights; i++)\n"
"    {\n"

"        vec3 lightVec = lightPositions[i] - v_frag_pos;\n"

"        float distance = length(lightVec);\n"

"        vec3 L = normalize(lightVec);\n"

"        vec3 R = reflect(-L, N);\n"

"        float attenuation = 1.0 / (\n"
"            1.0 +\n"
"            0.09 * distance +\n"
"            0.032 * distance * distance\n"
"        );\n"

"        float diff = max(dot(N, L), 0.0);\n"

"        diffuse += diff *lightColor * intensity * attenuation*3;  \n"


"        float specularStrength = 0.12;\n"

"        float spec = pow(max(dot(V, R), 0.0), 32.0);\n"

"       specular +=specularStrength *spec *lightColor *intensity *attenuation *8.0; \n"
"    }\n"
"    vec3 skyColor = vec3(0.16, 0.20, 0.28);\n"
"    vec3 groundColor = vec3(0.05, 0.05, 0.06);\n"

"    float hemiFactor = N.y * 0.5 + 0.5;\n"

"    vec3 ambient = mix(\n"
"        groundColor,\n"
"        skyColor,\n"
"        hemiFactor\n"
"    );\n"

"    vec4 baseColor; \n"

"    if(useTexture) \n"
"    { \n"

"        baseColor = texture(ourTexture, v_tex); \n"

"        if(baseColor.a < 0.01) discard; \n"

"    } \n"
"    else \n"
"    { \n"

"        baseColor = materialColor; \n"

"    } \n"

"    float shadow = ShadowCalculation(\n"
"        fragPosLightSpace,\n"
"        N,\n"
"        firstLightDir\n"
"    );\n"

"    vec3 lighting = ambient + diffuse + specular;\n"

"    lighting -= shadow * (diffuse + specular);\n"

"    lighting = clamp(lighting, 0.0, 2.0);\n"

"    FragColor = vec4(baseColor.rgb * lighting, baseColor.a); \n"

"}\n";


GLuint compileShader(std::string shaderCode, GLenum shaderType)
{
	GLuint shader = glCreateShader(shaderType);
	const char* code = shaderCode.c_str();
	glShaderSource(shader, 1, &code, NULL);
	glCompileShader(shader);

	GLchar infoLog[1024];
	GLint success;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(shader, 1024, NULL, infoLog);
		std::string t = "undetermined";
		if (shaderType == GL_VERTEX_SHADER) {
			t = "vertex shader";
		}
		else if (shaderType == GL_FRAGMENT_SHADER) {
			t = "fragment shader";
		}
		std::cout << "ERROR::SHADER_COMPILATION_ERROR of the " << t << ": " << shaderType << infoLog << std::endl;
	}
	return shader;
}

GLuint compileProgram(GLuint vertexShader, GLuint fragmentShader)
{
	GLuint programID = glCreateProgram();

	glAttachShader(programID, vertexShader);
	glAttachShader(programID, fragmentShader);
	glLinkProgram(programID);


	GLchar infoLog[1024];
	GLint success;
	glGetProgramiv(programID, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(programID, 1024, NULL, infoLog);
		std::cout << "ERROR::PROGRAM_LINKING_ERROR:  " << infoLog << std::endl;
	}
	return programID;
}