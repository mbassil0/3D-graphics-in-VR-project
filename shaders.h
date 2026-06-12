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

const std::string sourceFl =
"#version 330 core\n"
"out vec4 FragColor;\n"
"\n"
"in vec3 v_frag_pos;\n"
"in vec3 v_normal;\n"
"in vec2 v_tex;\n"
"in vec4 fragPosLightSpace;\n"
"\n"
"uniform sampler2D ourTexture;\n"
"uniform samplerCube skybox;\n"
"\n"
"uniform float reflectivity;\n"
"uniform bool useTexture;\n"
"uniform vec4 materialColor;\n"
"\n"
"uniform vec3 lightPositions[10];\n"
"uniform vec3 lightColor;\n"
"uniform int numLights;\n"
"\n"
"uniform vec3 viewPos;\n"
"\n"
"uniform float fogDensity;\n"
"uniform vec3 fogColor;\n"
"\n"
"void main()\n"
"{\n"
"    vec3 N = normalize(v_normal);\n"
"    vec3 V = normalize(viewPos - v_frag_pos);\n"
"\n"
"    vec4 baseColor;\n"
"\n"
"    if(useTexture)\n"
"    {\n"
"        baseColor = texture(ourTexture, v_tex);\n"
"\n"
"        if(baseColor.a < 0.01)\n"
"            discard;\n"
"    }\n"
"    else\n"
"    {\n"
"        baseColor = materialColor;\n"
"    }\n"
"\n"
"    vec3 diffuse = vec3(0.0);\n"
"    vec3 specular = vec3(0.0);\n"
"\n"
"    for(int i = 0; i < numLights; i++)\n"
"    {\n"
"        vec3 lightDir = lightPositions[i] - v_frag_pos;\n"
"        float distance = length(lightDir);\n"
"        vec3 L = normalize(lightDir);\n"
"\n"
"        float attenuation =\n"
"            1.0 / (1.0 + 0.02 * distance * distance);\n"
"\n"
"        float diff = max(dot(N, L), 0.0);\n"
"\n"
"        diffuse += diff * lightColor * attenuation;\n"
"\n"
"        vec3 H = normalize(L + V);\n"
"\n"
"        float spec = pow(max(dot(N, H), 0.0), 64.0);\n"
"\n"
"        specular += spec * lightColor * attenuation * 0.5;\n"
"    }\n"
"\n"
"    vec3 ambient = vec3(0.08);\n"
"\n"
"    vec3 litColor =\n"
"        baseColor.rgb * (ambient + diffuse)\n"
"        + specular;\n"
"\n"
"    vec3 I = normalize(v_frag_pos - viewPos);\n"
"    vec3 R = reflect(I, N);\n"
"\n"
"    vec3 reflectionColor = texture(skybox, R).rgb;\n"
"\n"
"    float fresnel = pow(\n"
"        1.0 - max(dot(N, V), 0.0),\n"
"        5.0\n"
"    );\n"
"\n"
"    float reflectionAmount =\n"
"        clamp(reflectivity + fresnel * 0.5, 0.0, 1.0);\n"
"\n"
"    reflectionAmount = reflectivity;\n"
"\n"
"    vec3 finalColor = mix(\n"
"        litColor,\n"
"        reflectionColor,\n"
"        reflectionAmount\n"
"    );\n"
"\n"
"    // =========================\n"
"    // BEER-LAMBERT DEPTH FOG\n"
"    // =========================\n"
"    // =========================\n"
"    // EXP2 HEIGHT FOG\n"
"    // =========================\n"
"\n"
"    float zs = length(viewPos - v_frag_pos);\n"
"\n"
"    float heightFactor = exp(-0.08 * max(v_frag_pos.y, 0.0));\n"
"\n"
"    float density = fogDensity * heightFactor;\n"
"\n"
"    float f = exp(-pow(density * zs, 2.0));\n"
"\n"
"    vec3 skyFogColor = mix(\n"
"        fogColor,\n"
"        vec3(0.65, 0.80, 1.00),\n"
"        0.5\n"
"    );\n"
"\n"
"    finalColor = mix(\n"
"        skyFogColor,\n"
"        finalColor,\n"
"        f\n"
"    );\n"
"\n"

"\n"
"    finalColor = pow(finalColor, vec3(1.0 / 2.2));\n"
"\n"
"    FragColor = vec4(finalColor, baseColor.a);\n"
"}\n";



const std::string  textVS =
"#version 330 core\n"
"\n"
"layout(location = 0) in vec4 vertex;\n"
"\n"
"out vec2 TexCoords;\n"
"\n"
"uniform mat4 projection;\n"
"\n"
"void main()\n"
"{\n"
"    gl_Position = projection * vec4(vertex.xy, 0.0, 1.0);\n"
"    TexCoords = vertex.zw;\n"
"}\n";
const  std::string textFS =
"#version 330 core\n"
"\n"
"in vec2 TexCoords;\n"
"out vec4 FragColor;\n"
"\n"
"uniform sampler2D textTexture;\n"
"\n"
"void main()\n"
"{\n"
"    float alpha = texture(textTexture, TexCoords).r;\n"
"    FragColor = vec4(1.0, 1.0, 1.0, alpha);\n"
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
