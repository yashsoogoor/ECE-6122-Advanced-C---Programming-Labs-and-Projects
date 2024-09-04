/*

Author: Yashwanth Soogoor
Class : ECE 6122 Section A
Last Date Modified : 10/27/2023
Description :
This file sets up the keyboard controls for the Suzane objects application. 

*/





// Include GLFW
#include <GLFW/glfw3.h>
extern GLFWwindow* window; // The "extern" keyword here is to access the variable "window" declared in tutorialXXX.cpp. This is a hack to keep the tutorials simple. Please avoid this.

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

#include "controls.hpp"

glm::mat4 ViewMatrix;
glm::mat4 ProjectionMatrix;

glm::mat4 getViewMatrix() 
{
	return ViewMatrix;
}
glm::mat4 getProjectionMatrix() 
{
	return ProjectionMatrix;
}


glm::vec3 origin = glm::vec3(0, 0, 0);
// Initial position : on +Z
glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f);
// Initial horizontal angle : toward -Z
float phi = 3.14f;
// Initial vertical angle : none
float theta = 1.0f;
// Initial Field of View
float initialFoV = 45.0f;

float radius = 10.0f;

float PI = 3.14f;
float deltaTheta = 0.001f * PI; 
float deltaPhi = 0.001f * PI; 
float deltaRadius = 0.01f;


//Function to set up keyboard inputs for controlling theta, phi, and radius
void computeMatricesFromInputs() 
{


	// Up vector
	glm::vec3 up = glm::vec3(0.0f, 0.0f, 1.0f);

	

	// Rotate up
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) 
	{
		theta -= deltaTheta;

		// Ensure theta is not less than 0
		if (theta <= 0.001f) 
		{
			theta = 0.001f;
		}

		// Ensure theta is not greater than pi
		if (theta >= glm::pi<float>()) 
		{
			theta = glm::pi<float>();
		}
	}
	// Rotate down
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) 
	{
		theta += deltaTheta;
		// Ensure theta is not less than 0
		if (theta < 0.001f) {
			theta = 0.001f;
		}

		// Ensure theta is not greater than pi
		if (theta > glm::pi<float>() ) {
			theta = glm::pi<float>() ;
		}
	}

	// Rotate right
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) 
	{
		phi += deltaPhi;

		// Ensure phi is within [0, 2 * PI)
		if (phi >= (2 * glm::pi<float>())) 
		
		{
			phi -= (2 * glm::pi<float>());
		}
	}
	// Rotate left
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) 
	{
		phi -= deltaPhi;
		phi = (phi < 0) ? (2 * glm::pi<float>()) + phi : phi;
	}
	// Move closer
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) 
	{
		if (radius >= 0) 
		{
			radius -= deltaRadius;
		}
		else 
		{
			radius = 0;
		}

	}
	// Move farther
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) 
	{
		radius += deltaRadius;
	}

	position.x = radius * sin(theta) * cos(phi);
	position.y = radius * sin(theta) * sin(phi);
	position.z = radius * cos(theta);


	float FoV = initialFoV;
	//Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
	ProjectionMatrix = glm::perspective(glm::radians(FoV), 4.0f / 3.0f, 0.1f, 100.0f);
	//Camera matrix;

	
	ViewMatrix = glm::lookAt(
		position,           // Camera is here
		origin, // and looks here : at the same position, plus "direction"
		up                  // Head is up (set to 0,-1,0 to look upside-down)
	);

	// For the next frame, the "last time" will be "now"
	//lastTime = currentTime;



}