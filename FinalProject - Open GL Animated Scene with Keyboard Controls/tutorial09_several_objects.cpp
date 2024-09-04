/*

Author: Yashwanth Soogoor
Class : ECE 6122 Section A
Last Date Modified : 12/08/2023
Description :
This file sets up the framework and functions for creating Suzanne objects in an animated scene which float/rotate about randomly with a user key press. 

*/


#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <thread>


// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <GLFW/glfw3.h>
GLFWwindow* window;

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
using namespace glm;

#include <common/shader.hpp>
#include <common/texture.hpp>
#include <common/controls.hpp>
#include <common/objloader.hpp>
#include <common/vboindexer.hpp>
#include <mutex>

std::mutex suzanneMutex;


std::vector<glm::vec3> suzannePositions;
std::vector<glm::vec3> suzanneVelocities;
std::vector<glm::quat> suzanneRotations;
bool enableMotion = false;

#include <cmath>
#include <ctime>
#include <cstdlib>


#include <random>


// Struct for lighting info
struct Light 
{
	glm::vec3 position;
	float intensity;
};

std::vector<Light> suzanneLights;

// Random light intensity
float randomIntensity() 
{
	return static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
}

void initializeSuzanneLights() 
{
	// Resize the vector to hold lights for each head
	suzanneLights.resize(9);

	// Initialize lights
	for (int i = 0; i < suzanneLights.size(); ++i) 
	{
		suzanneLights[i].position = suzannePositions[i];
		suzanneLights[i].intensity = randomIntensity();
	}
}

// Function to update lights
void updateSuzanneLights() 
{
	while (true) 
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(8));

		if (enableMotion) 
		{
			std::lock_guard<std::mutex> lock(suzanneMutex);
			for (int i = 0; i < suzanneLights.size(); ++i) 
			{
				suzanneLights[i].intensity = randomIntensity();
			}
		}
	}
}

// Function to generate a float
float randomFloat() 
{
	return static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * 2.0f - 1.0f;
}

//Function to initialize suzanne heads
void initializeSuzanne() 
{
	// Seed the random number generator
	srand(static_cast<unsigned>(time(nullptr)));

	// Resize vectors to hold 4 heads
	suzannePositions.resize(9);
	suzanneVelocities.resize(9);
	suzanneRotations.resize(9);


	// Initialize positions along the z-axis starting from z=2
	for (int i = 0; i < suzannePositions.size(); ++i) 
	{
		suzannePositions[i] = glm::vec3(0.0f, 2.0f * i - 8.0f, 1.5f);
	}

	// Initialize velocities randomly
	for (int i = 0; i < suzanneVelocities.size(); ++i) 
	{
		suzanneVelocities[i] = glm::vec3(randomFloat(), randomFloat(), randomFloat()) * 0.1f;
	}


}


// This function updates the functions of the Suzanne heads continuously when the user enters G and makes them bounce off each other and the floor
void updateSuzannePositions() 
{
	while (true) 
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(16));  

		if (enableMotion) 
		{
			std::vector<glm::vec3> positionsCopy;
			std::vector<glm::vec3> velocitiesCopy;
			std::vector<glm::quat> rotationsCopy;
			

			{
				std::lock_guard<std::mutex> lock(suzanneMutex);
				positionsCopy = suzannePositions;
				velocitiesCopy = suzanneVelocities;
				rotationsCopy = suzanneRotations;

				
			}

			// Hitting each other
			for (int i = 0; i < positionsCopy.size(); ++i) 
			{
				positionsCopy[i] += velocitiesCopy[i];

				// Hitting Floor
				if (positionsCopy[i].z < 1.0f) 
				{
					positionsCopy[i].z = 1.0f;
					velocitiesCopy[i].z = -velocitiesCopy[i].z;
				}

				// Boundaries to not float off forever
				float wallBound = 10.0f;
				if (positionsCopy[i].x < -wallBound || positionsCopy[i].x > wallBound) 
				{
					velocitiesCopy[i].x = -velocitiesCopy[i].x;
				}
				if (positionsCopy[i].y < -wallBound || positionsCopy[i].y > wallBound) 
				{
					velocitiesCopy[i].y = -velocitiesCopy[i].y;
				}
				if (positionsCopy[i].z < -wallBound || positionsCopy[i].z > wallBound) 
				{
					velocitiesCopy[i].z = -velocitiesCopy[i].z;
				}

				// Hitting each other 
				for (int j = 0; j < positionsCopy.size(); ++j) 
				{
					if (i != j) 
					{
						glm::vec3 delta = positionsCopy[j] - positionsCopy[i];
						float dist = glm::length(delta);
						float minDistance = 1.08f;  // Min distance for contact
						if (dist < minDistance) 
						{
							glm::vec3 norm = glm::normalize(delta);
							glm::vec3 relVel = velocitiesCopy[j] - velocitiesCopy[i];
							float speed = glm::dot(relVel, norm);

							if (speed < 0.0f) 
							{
								// Bounce 
								float r = 0.8f; 
								glm::vec3 imp = (1.0f + r) * speed * norm;
								velocitiesCopy[i] -= imp;
								velocitiesCopy[j] += imp;
							}
						}

					}

				}

				glm::quat deltaRotation = glm::angleAxis(glm::radians(randomFloat() * 2.0f - 1.0f), glm::vec3(1.0f, 0.0f, 0.0f))
					* glm::angleAxis(glm::radians(randomFloat() * 2.0f - 1.0f), glm::vec3(0.0f, 1.0f, 0.0f))
					* glm::angleAxis(glm::radians(randomFloat() * 2.0f - 1.0f), glm::vec3(0.0f, 0.0f, 1.0f));

				rotationsCopy[i] = deltaRotation * rotationsCopy[i];


				float maxSpeed = .10f;  
				float currentSpeed = glm::length(velocitiesCopy[i]);
				if (currentSpeed > maxSpeed) {
					velocitiesCopy[i] = glm::normalize(velocitiesCopy[i]) * maxSpeed;
				}


			}

			{
				std::lock_guard<std::mutex> lock(suzanneMutex);
				suzannePositions = positionsCopy;
				suzanneVelocities = velocitiesCopy;
				suzanneRotations = rotationsCopy;
			}
		}
	}
}






//Function to create and render objects and toggle light; returns 0 upon completion; G press toggles animation to start and heads to float

int main(void)
{
	//Variables for passing uniform variable to fragnment shader to toggle lights
	bool L = true;
	bool keyLPressed = false;
	double lastLKeyPressTime = 0.0;
	bool keyGPressed = false;
	double lastGKeyPressTime = 0.0;
	double toggleDelay = 0.5; // Set delay time in seconds





	// Initialise GLFW
	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW\n");
		getchar();
		return -1;
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Open a window and create its OpenGL context
	window = glfwCreateWindow(1024, 768, "Tutorial 09 - Rendering several models", NULL, NULL);
	if (window == NULL) 
	{
		fprintf(stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n");
		getchar();
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	// Initialize GLEW
	glewExperimental = true; // Needed for core profile
	if (glewInit() != GLEW_OK) 
	{
		fprintf(stderr, "Failed to initialize GLEW\n");
		getchar();
		glfwTerminate();
		return -1;
	}

	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
	// Hide the mouse and enable unlimited mouvement
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// Set the mouse at the center of the screen
	glfwPollEvents();
	glfwSetCursorPos(window, 1024 / 2, 768 / 2);

	// Dark blue background
	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

	// Enable depth test
	glEnable(GL_DEPTH_TEST);
	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS);

	// Cull triangles which normal is not towards the camera
	//glEnable(GL_CULL_FACE);

	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	// Create and compile our GLSL program from the shaders
	GLuint programID = LoadShaders("StandardShading.vertexshader", "StandardShading.fragmentshader");

	// Get a handle for our "MVP" uniform
	GLuint MatrixID = glGetUniformLocation(programID, "MVP");
	GLuint ViewMatrixID = glGetUniformLocation(programID, "V");
	GLuint ModelMatrixID = glGetUniformLocation(programID, "M");

	// Load the texture
	GLuint Texture = loadDDS("uvmap.DDS");

	// Get a handle for our "myTextureSampler" uniform
	GLuint TextureID = glGetUniformLocation(programID, "myTextureSampler");

	// Read our .obj file
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec2> uvs;
	std::vector<glm::vec3> normals;
	bool res = loadOBJ("suzanne.obj", vertices, uvs, normals);

	std::vector<unsigned short> indices;
	std::vector<glm::vec3> indexed_vertices;
	std::vector<glm::vec2> indexed_uvs;
	std::vector<glm::vec3> indexed_normals;
	indexVBO(vertices, uvs, normals, indices, indexed_vertices, indexed_uvs, indexed_normals);

	// Load it into a VBO

	GLuint vertexbuffer;
	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, indexed_vertices.size() * sizeof(glm::vec3), &indexed_vertices[0], GL_STATIC_DRAW);

	GLuint uvbuffer;
	glGenBuffers(1, &uvbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
	glBufferData(GL_ARRAY_BUFFER, indexed_uvs.size() * sizeof(glm::vec2), &indexed_uvs[0], GL_STATIC_DRAW);

	GLuint normalbuffer;
	glGenBuffers(1, &normalbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
	glBufferData(GL_ARRAY_BUFFER, indexed_normals.size() * sizeof(glm::vec3), &indexed_normals[0], GL_STATIC_DRAW);

	// Generate a buffer for the indices as well
	GLuint elementbuffer;
	glGenBuffers(1, &elementbuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned short), &indices[0], GL_STATIC_DRAW);

	// Get a handle for our "LightPosition" uniform
	glUseProgram(programID);


	GLuint LightID = glGetUniformLocation(programID, "LightPosition_worldspace");

	// For speed computation
	double lastTime = glfwGetTime();
	int nbFrames = 0;

	//Create vertices for 2 green triangles 
	static const GLfloat g_vertex_buffer_data[] =
	{
		-300.0f, -300.0f, 0.0f,
		 300.0f, -300.0f, 0.0f,
		 300.0f,  300.0f, 0.0f,
	};


	static const GLfloat g_vertex_buffer_data2[] =
	{
		-300.0f, -300.0f, 0.0f,  // Vertex 1 
		 300.0f,  300.0f, 0.0f,  // Vertex 3 
		-300.0f,  300.0f, 0.0f,  // Vertex 4
	};


	/*
	// Initialize Suzanne head positions
	suzannePositions.push_back(glm::vec3(1.90f, 0.0f, 0.98f));
	suzannePositions.push_back(glm::vec3(1.90f, 0.0f, 0.98f));
	suzannePositions.push_back(glm::vec3(1.90f, 0.0f, 0.98f));
	suzannePositions.push_back(glm::vec3(1.90f, 0.0f, 0.98f));
	*/


	initializeSuzanne();
	// Initialize Suzanne lights
	initializeSuzanneLights();



	// Create thread for motion calculations
	std::thread motionThread(updateSuzannePositions);
	// Create thread for light calculations
	std::thread lightThread(updateSuzanneLights); 

	GLuint TriangleTexture = loadDDS("uvmap.DDS");

	//do While loop for rendering suzanne heads
	do 
	{

		//std::lock_guard<std::mutex> lock(suzanneMutex);

		// Keyboard control for starting Suzanne head random floating and rotating
		if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS)
			if (!keyGPressed || (glfwGetTime() - lastGKeyPressTime) >= toggleDelay)
			{
				enableMotion = !enableMotion;
				keyGPressed = true;
				lastGKeyPressTime = glfwGetTime();
			}
			else
			{
				keyGPressed = false;
			}


		//Keyboard control for toggling lights 
		if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS) 
		{
			if (!keyLPressed || (glfwGetTime() - lastLKeyPressTime) >= toggleDelay)
			{
				L = !L;
				keyLPressed = true;
				lastLKeyPressTime = glfwGetTime();
			}
		}
		else
		{
			keyLPressed = false;
		}

		// Set the uniform value in the shader
		glUniform1i(glGetUniformLocation(programID, "L"), L ? 1 : 0);


		// Measure speed
		double currentTime = glfwGetTime();
		nbFrames++;
		if (currentTime - lastTime >= 1.0) 
		{ // If last prinf() was more than 1sec ago
			// printf and reset
			printf("%f ms/frame\n", 1000.0 / double(nbFrames));
			nbFrames = 0;
			lastTime += 1.0;
		}

		// Clear the screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		// Compute the MVP matrix from keyboard and mouse input
		computeMatricesFromInputs();
		glm::mat4 ProjectionMatrix = getProjectionMatrix();
		glm::mat4 ViewMatrix = getViewMatrix();


		// Loop over all Suzanne objects
		for (int i = 0; i < suzannePositions.size(); ++i) 
		{
			// Use shader
			glUseProgram(programID);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, Texture);
			glUniform1i(TextureID, 0);

			std::lock_guard<std::mutex> lock(suzanneMutex);

			glm::vec3 lightPos = suzannePositions[i];
			glUniform3f(LightID, lightPos.x, lightPos.y, lightPos.z);
			glUniform1f(glGetUniformLocation(programID, "LightIntensity"), suzanneLights[i].intensity); 
			glUniformMatrix4fv(ViewMatrixID, 1, GL_FALSE, &ViewMatrix[0][0]);

			
			glm::mat4 ModelMatrix = glm::mat4(1.0);
			ModelMatrix = glm::translate(ModelMatrix, suzannePositions[i]);
			ModelMatrix = glm::rotate(ModelMatrix, glm::radians(90.0f), glm::vec3(0.0f, -1.0f, 0.0f));
			ModelMatrix = glm::rotate(ModelMatrix, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, -1.0f));
			ModelMatrix = ModelMatrix * glm::mat4_cast(suzanneRotations[i]);

			glm::mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;

		
			glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
			glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);

		
			glEnableVertexAttribArray(0);
			glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

			glEnableVertexAttribArray(1);
			glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

			
			glEnableVertexAttribArray(2);
			glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
			glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

			
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);

			// Draw triangles
			glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_SHORT, (void*)0);

			glDisableVertexAttribArray(0);
			glDisableVertexAttribArray(1);
			glDisableVertexAttribArray(2);

			glBindTexture(GL_TEXTURE_2D, 0);
		}


		////////////////

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, Texture);
		glUniform1i(TextureID, 1);

		// Create a VBO for the rectangle
		GLuint rectVertexBuffer;
		glGenBuffers(1, &rectVertexBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, rectVertexBuffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

		// Bind the VBO and set up vertex attribute pointers for the rectangle
		glBindBuffer(GL_ARRAY_BUFFER, rectVertexBuffer);
		glEnableVertexAttribArray(0);  // Attribute 0 is for vertices
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		// Create a new ModelMatrix for the rectangle
		glm::mat4 ModelMatrixRect = glm::mat4(1.0);
		// Apply transformations to ModelMatrixRect as needed to position and scale the rectangle

		// Calculate the MVP matrix for the rectangle
		glm::mat4 MVPRect = ProjectionMatrix * ViewMatrix * ModelMatrixRect;

		// Send the transformation to the shader
		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVPRect[0][0]);
		glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrixRect[0][0]);


		// Render the rectangle
		glDrawArrays(GL_TRIANGLES, 0, 3);  // Assuming 3 vertices for the triangle



		////////////////

			// Create a VBO for the rectangle

		glGenBuffers(1, &rectVertexBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, rectVertexBuffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data2), g_vertex_buffer_data2, GL_STATIC_DRAW);

		// Bind the VBO and set up vertex attribute pointers for the rectangle
		glBindBuffer(GL_ARRAY_BUFFER, rectVertexBuffer);
		glEnableVertexAttribArray(0);  // Attribute 0 is for vertices
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		// Create a new ModelMatrix for the rectangle
		glm::mat4 ModelMatrixRect2 = glm::mat4(1.0);
		// Apply transformations to ModelMatrixRect as needed to position and scale the rectangle

		// Calculate the MVP matrix for the rectangle
		glm::mat4 MVPRect2 = ProjectionMatrix * ViewMatrix * ModelMatrixRect2;

		// Send the transformation to the shader
		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVPRect2[0][0]);
		glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrixRect2[0][0]);


		// Render the rectangle
		glDrawArrays(GL_TRIANGLES, 0, 3);  // Assuming 3 vertices for the triangle


		////////////////



		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		glDisableVertexAttribArray(2);
		glBindTexture(GL_TEXTURE_2D, 1);

		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();

	} // Check if the ESC key was pressed or the window was closed
	while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
		glfwWindowShouldClose(window) == 0);

	motionThread.join();
	lightThread.join();

	// Cleanup VBO and shader
	glDeleteBuffers(1, &vertexbuffer);
	glDeleteBuffers(1, &uvbuffer);
	glDeleteBuffers(1, &normalbuffer);
	glDeleteBuffers(1, &elementbuffer);
	glDeleteProgram(programID);
	glDeleteTextures(1, &Texture);
	glDeleteVertexArrays(1, &VertexArrayID);

	// Close OpenGL window and terminate GLFW
	glfwTerminate();

	return 0;
}

