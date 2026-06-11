
#include "util.h"


Camera camera(glm::vec3(0.0, 3.0, 8.0));


void processInput(GLFWwindow* window, UserCmd* cmd, InputMgr* input_mgr) {
	// Use the cameras class to change the parameters of the camera
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
		camera.ProcessKeyboardMovement(LEFT, 0.1);
	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
		camera.ProcessKeyboardMovement(RIGHT, 0.1);

	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
		camera.ProcessKeyboardMovement(FORWARD, 0.1);
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
		camera.ProcessKeyboardMovement(BACKWARD, 0.1);

	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboardRotation(1, 0.0, 1);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboardRotation(-1, 0.0, 1);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboardRotation(0.0, 1.0, 1);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboardRotation(0.0, -1.0, 1);



	if (glfwGetKey(window, GLFW_KEY_KP_8) == GLFW_PRESS)
	{
		cmd->forward = 1;
	}
	if (glfwGetKey(window, GLFW_KEY_KP_2) == GLFW_PRESS)
	{
		cmd->backward = 1;
	}
	if (glfwGetKey(window, GLFW_KEY_KP_6) == GLFW_PRESS)
	{
		cmd->right = 1;
	}
	if (glfwGetKey(window, GLFW_KEY_KP_4) == GLFW_PRESS)
	{
		cmd->left = 1;
	}

	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
	{
		cmd->jump = 1;
	}

	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
	{
		cmd->shoot = 1;
	}


	/*if (glfwGetKey(window, GLFW_KEY_TAB) == GLFW_PRESS)
	{
		if (input_mgr->thirdPersonView == 1) { input_mgr->thirdPersonView = 0; }

		if (input_mgr->thirdPersonView == 0) { input_mgr->thirdPersonView = 1; }

	}*/

	bool tabNow =
		glfwGetKey(window, GLFW_KEY_TAB) == GLFW_PRESS;

	if (tabNow && !input_mgr->tabPressedLastFrame)
	{
		input_mgr->thirdPersonView =
			!input_mgr->thirdPersonView;

		//printf(" we changing to %d \n", input_mgr->thirdPersonView);
	}

	input_mgr->tabPressedLastFrame = tabNow;




}

/*  
void  RenderableMgr::performGravity(Renderable* renderable)
{
	
	
	float groundDist = getGroundDistance(renderable->position);

	printf("got ground dist %f \n ", groundDist);
	if (groundDist > 0  )
		renderable->position[1] -= 0.25;

	if (renderable->position[1] < 0.0f) //we touched the ground
		renderable->position[1] = 0.0f; 
	
}*/


void RenderableMgr::performGravity(Renderable* renderable)
{
	static float verticalSpeed = 0.0f;

	// gravity pulls downward every frame
	verticalSpeed -= 0.015f;



	float initial_z = renderable->position[1];


	// move player
	renderable->position[1] += verticalSpeed;

	Shape* shapeBelow;
	// collision with ground
	float groundDist = getGroundDistance(renderable->position, shapeBelow );

	if (groundDist <= 0.0f)
	{
		renderable->position[1] -= groundDist;

		verticalSpeed = 0.00001f;
	}
}

void RenderableMgr::updateGameLogic(Renderable* renderable,
	UserCmd cmd,
	InputMgr* out)
{
	glm::vec3 forward;

	forward.x =
		cos(glm::radians(camera.Yaw)) *
		cos(glm::radians(camera.Pitch));

	forward.y = 0.0f;

	forward.z =
		sin(glm::radians(camera.Yaw)) *
		cos(glm::radians(camera.Pitch));

	forward = glm::normalize(forward);
	glm::vec3 right =
		glm::normalize(
			glm::cross(forward, glm::vec3(0.0f, 1.0f, 0.0f))
		);



	glm::vec3 prev_pos = renderable->position;

	// movement
	if (cmd.forward)
		renderable->position += forward * 0.2f;
	if (cmd.backward)
		renderable->position -= forward * 0.2f;
	if (cmd.right)
		renderable->position += right * 0.2f;
	if (cmd.left)
		renderable->position -= right * 0.2f;


	// jumping
	static float verticalSpeed = 0.0f;
	Shape* shapeBelow= nullptr;
	float groundDist = getGroundDistance(renderable->position, shapeBelow);
	//printf("grd %f   \n ", groundDist);
	// jump


	float  distOffset = 1.51; // dist between center of bazooka and ground
	if (cmd.jump && (groundDist- distOffset )<= 0.05f)
	{

		printf("jumpinng \n");
		verticalSpeed = 0.35f;
	}
	shapeBelow = nullptr;


	if (groundDist <=-0.5) //nothing below
		verticalSpeed = 0.0f;

	//printf("ground dist %f \n", getGroundDistance(renderable->position, shapeBelow));
	groundDist = getGroundDistance(renderable->position, shapeBelow);
	if (groundDist- distOffset > 0.015 && shapeBelow != nullptr) {
		// gravity
		verticalSpeed -= 0.015f;
	}
	// move
	renderable->position[1] += verticalSpeed;


	if (shapeBelow == nullptr)
		renderable->position[1] = 0.0;


	// recompute AFTER movement
	//groundDist = getGroundDistance(renderable->position, shapeBelow);

	// landed
	/* if (groundDist > 0.001f)
	{
		renderable->position[1] -= groundDist;

		verticalSpeed = 0.0f;
	}*/


	if (isInsideShape(renderable->position))
	{
		renderable->position = prev_pos;
		verticalSpeed = 0.0f;
	}
}

bool  RenderableMgr::isInsideShape(glm::vec3 pos)
{



	for (int i = 0; i < sceneShapes.size(); i++)
	{
		Renderable* r = &renderables[sceneShapes[i].renderableIndex];
		glm::vec3 worldMin = sceneShapes[i].min * r->scale + r->position;

		glm::vec3 worldMax = sceneShapes[i].max * r->scale + r->position;

		if (
			pos.x > worldMin.x &&
			pos.x < worldMax.x &&

			pos.z > worldMin.z &&
			pos.z < worldMax.z
			&& pos.y > worldMin.y &&
			pos.y < worldMax.y

			)
		{
			return true;
		}

	}
	return false;
}


//todo rename getground distance
//returns the distance between the player and the closest object below him
float RenderableMgr::getGroundDistance(glm::vec3 pos, Shape *&shapeBelow)
{
	float minDist = 999999999999;
	
	for (int i = 0; i < sceneShapes.size(); i++)
	{
		if (sceneShapes[i].renderableIndex == -1)
		{
			printf("error  getGround index -1 \n ");
			continue;
		}
		
		Renderable* r = &renderables[sceneShapes[i].renderableIndex];


		glm::vec3 worldMin = sceneShapes[i].min * r->scale + r->position;

		glm::vec3 worldMax = sceneShapes[i].max * r->scale + r->position;
		//printf(" got %f %f %f \n", r-> position[0], r->position[1], r->position[2]);
		if (
			pos.x > worldMin.x &&
			pos.x < worldMax.x &&

			pos.z > worldMin.z &&
			pos.z < worldMax.z
			)
		{
			
			if ( (pos.y - worldMax.y) < minDist )
			{
				minDist = pos.y - worldMax.y;
				
			}


			if(pos.y - worldMax.y > 0 )
				shapeBelow = &sceneShapes[i];

			/*printf(
				"inside object: ground=%f dist=%f\n",
				worldMax.y,
				pos.y - worldMax.y
			);*/
		}
	}


	if (shapeBelow == nullptr)
		minDist = -100.0f;
	
	
//	printf("fffinito %f \n\n", minDist);
	return minDist; 

}
