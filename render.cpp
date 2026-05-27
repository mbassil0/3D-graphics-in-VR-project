
#include "render.h"


void RenderScene(GLFWwindow* window, std::vector<Renderable>* renderables, ShadowMap shadowMap, glm::mat4 view, glm::mat4 perspective, GLuint program, GLint colorLoc, GLint useTextureLoc, GLint u_texture, std::vector<glm::vec3> light_pos)
{
	int display_w, display_h;

	glfwGetFramebufferSize(
		window,
		&display_w,
		&display_h
	);

	glViewport(
		0,
		0,
		display_w,
		display_h
	);

	for (int i = 0;
		i < renderables->size();
		i++)
	{
		glBindVertexArray(
			renderables->at(i)
			.VAO
		);

		glUseProgram(program);

		glUniform1i(
			glGetUniformLocation(program, "numLights"),
			light_pos.size()
		);

		for (int l = 0; l < light_pos.size(); l++)
		{
			std::string posName =
				"lightPositions[" + std::to_string(l) + "]";

			glUniform3fv(
				glGetUniformLocation(program, posName.c_str()),
				1,
				glm::value_ptr(light_pos[l])
			);
		}

		glUniform3f(
			glGetUniformLocation(
				program,
				"lightColor"
			),
			1.0f,
			1.0f,
			1.0f
		);

		glUniform3fv(
			glGetUniformLocation(
				program,
				"viewPos"
			),
			1,
			glm::value_ptr(
				camera.Position
			)
		);

		// =========================
		// SHADOW UNIFORMS
		// =========================

		glUniformMatrix4fv(
			glGetUniformLocation(
				program,
				"lightSpaceMatrix"
			),
			1,
			GL_FALSE,
			glm::value_ptr(
				shadowMap.lightSpaceMatrix
			)
		);

		glActiveTexture(GL_TEXTURE2);

		glBindTexture(
			GL_TEXTURE_2D,
			shadowMap.depthMap
		);

		glUniform1i(
			glGetUniformLocation(
				program,
				"shadowMap"
			),
			2
		);

		// =========================
		// MATRICES
		// =========================

		glm::mat4 model = glm::mat4(1.0f);

		model = glm::translate(model, renderables->at(i).position);

		 model = glm::rotate(model, glm::radians(renderables->at(i).rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));

		model = glm::rotate(model,glm::radians(renderables->at(i).rotation.y),glm::vec3(0, 1, 0));

		model = glm::rotate(model, glm::radians(renderables->at(i).rotation.z), glm::vec3(0, 0, 1));

		model = glm::scale(model, renderables->at(i).scale);


		//glm::mat4 itM =	glm::inverseTranspose(model);
		glm::mat4 itM =glm::mat4(glm::transpose(glm::inverse(glm::mat3(model))));

		glUniformMatrix4fv(
			glGetUniformLocation(
				program,
				"M"
			),
			1,
			GL_FALSE,
			glm::value_ptr(model)
		);

		glUniformMatrix4fv(
			glGetUniformLocation(
				program,
				"itM"
			),
			1,
			GL_FALSE,
			glm::value_ptr(itM)
		);

		glUniformMatrix4fv(
			glGetUniformLocation(
				program,
				"V"
			),
			1,
			GL_FALSE,
			glm::value_ptr(view)
		);

		glUniformMatrix4fv(
			glGetUniformLocation(
				program,
				"P"
			),
			1,
			GL_FALSE,
			glm::value_ptr(perspective)
		);

		// =========================
		// TEXTURES
		// =========================

		glActiveTexture(GL_TEXTURE0);

		glUniform1i(u_texture, 0);

		for (auto& tr :
			renderables->at(i)
			.material
			.textureRanges)
		{
			if (renderables->at(i).destroyed)
			{
				glUniform4f(
					colorLoc,
					0.05f,
					0.05f,
					0.05f,
					1.0f
				);
			}
			else {
				glUniform4fv(
					colorLoc,
					1,
					glm::value_ptr(
						tr.color
					)
				);
			}

			if (tr.hasTexture)
			{
				glBindTexture(
					GL_TEXTURE_2D,
					tr.texture.id
				);

				glUniform1i(
					useTextureLoc,
					1
				);
			}
			else
			{
				glBindTexture(
					GL_TEXTURE_2D,
					0
				);

				glUniform1i(
					useTextureLoc,
					0
				);
			}

			glDrawArrays(
				GL_TRIANGLES,
				tr.startVertex,
				tr.vertexCount
			);
		}
	}
	glEnable(GL_CULL_FACE);

	glDepthMask(GL_TRUE);
	glDepthFunc(GL_LESS);

}


void RenderSkybox(Shader cubeMapShader, GLuint cubeMapTexture, Object cubeMap, glm::mat4 view, glm::mat4 perspective)
{
	glDepthFunc(GL_LEQUAL);
	glDepthMask(GL_FALSE);

	glDisable(GL_CULL_FACE);

	cubeMapShader.use();

	glm::mat4 skyboxView = glm::mat4(glm::mat3(view));

	cubeMapShader.setMatrix4("V", skyboxView);
	cubeMapShader.setMatrix4("P", perspective);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMapTexture);

	cubeMapShader.setInteger("cubemapSampler", 0);

	cubeMap.draw(cubeMapShader);

	glDepthMask(GL_TRUE);
	glDepthFunc(GL_LESS);
}

void renderParticleSystem(ExplosionSystem* particleSystem, glm::mat4 view, glm::mat4 perspective)
{
	particleSystem->shader.use();
	particleSystem->shader.setMatrix4("V", view);
	particleSystem->shader.setMatrix4("P", perspective);
	particleSystem->shader.setVector3f(
		"cameraRight",
		view[0][0],
		view[1][0],
		view[2][0]
	);

	particleSystem->shader.setVector3f(
		"cameraUp",
		view[0][1],
		view[1][1],
		view[2][1]
	);
	particleSystem->Update(camera.Position);
	particleSystem->Render();
}