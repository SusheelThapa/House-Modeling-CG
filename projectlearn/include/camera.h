#ifndef CAMERA_H
#define CAMERA_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "glm/ext/matrix_transform.hpp"
#include "glm/fwd.hpp"

#include <math.h>

#include <vector>

/*Defining Camera Position*/
enum Camera_Movement
{
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT,
	UP,
	DOWN,

	/*Circular movement*/
	CRIGHT,
	CLEFT
};

/*Constants that defines various parameter */
const float YAW = -90.0f; // Default horizontal rotation angle
const float PITCH = 0.0f; // Default vertical rotation angle
const float SPEED = 7.5f;
const float SENSITIVITY = 0.05f; // Mouse sensitivity
const float ZOOM = 45.0f;		 // Initial FOV(Field of View)
const float EULERFACTOR = 30.f;	 // Factor for converting mouse movement

class Camera
{
public:
	/*Camera Attributes*/
	glm::vec3 Position;
	glm::vec3 Front;
	glm::vec3 Up;
	glm::vec3 Right;
	glm::vec3 WorldUp;
	bool otherlookat = false;

	/*Euler Angles*/
	float Yaw;
	float Pitch;

	/*Camera Options*/
	float MovementSpeed;
	float MouseSensitivity;
	float Zoom;

	/**
	 * Initialize the camera's properties based on the arguments passed
	 */
	Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, -35.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH) : MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
	{
		Front = glm::vec3(0.0f, 0.0f, 35.f);
		Position = position;
		WorldUp = up;
		Yaw = yaw;
		Pitch = PITCH;

		/*Recalculated the camera's orientation vector based on 'Yew' and 'Pitch' values*/
		updateCameraVectors();
	}

	/*Return the view matrix of the camera*/
	glm::mat4 GetViewMatrix()
	{
		return glm::lookAt(Position, Position + Front, Up);
	}

	/**
	 * Implementations of camera movements logic based
	 * in the specified 'Camera_Movement' direction and the
	 * provided 'deltaTime'
	 *
	 * It enables us to move, rotate the camera in the specified direction
	 * with specified speed and sensitivity
	 */
	void ProcessKeyboard(Camera_Movement direction, float deltaTime)
	{
		float velocity = MovementSpeed * deltaTime;

		if (direction == FORWARD)
			Position += Front * velocity;

		if (direction == BACKWARD)
			Position -= Front * velocity;

		if (direction == LEFT)
			Position -= Right * velocity;

		if (direction == RIGHT)
			Position += Right * velocity;

		if (direction == CLEFT)
		{
			Yaw -= deltaTime * EULERFACTOR;
			updateCameraVectors();
		}

		if (direction == CRIGHT)
		{

			Yaw += deltaTime * EULERFACTOR;
			updateCameraVectors();
		}

		if (direction == UP)
		{
			Position.y += velocity;
		}

		if (direction == DOWN)
		{
			Position.y -= velocity;
		}
	}

	/**
	 * Handle mouse input and update the camera's orientation
	 * accordingly, allowing user to look around in and 3D environment by moving
	 * the mouse.
	 */
	void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true)
	{
		xoffset *= MouseSensitivity;
		yoffset *= MouseSensitivity;

		Yaw += xoffset;
		Pitch += yoffset;

		if (constrainPitch)
		{
			if (Pitch > 89.0f)
				Pitch = 89.0f;
			if (Pitch < -89.0f)
				Pitch = -89.0f;
		}

		updateCameraVectors();
	}

	/*Updated the camera's zoom level based on the mouse scroll wheel movement*/
	void ProcessMouseScroll(float yoffset)
	{
		Zoom -= (float)yoffset;

		if (Zoom < 1.0f)
			Zoom = 1.0f;
		if (Zoom > 45.0f)
			Zoom = 45.0f;
	}

private:
	/**
	 * Update the camera's orientation and direction based on the current
	 * Yaw and Pitch angles
	*/
	void updateCameraVectors()
	{
		glm::vec3 front;
		front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
		front.y = sin(glm::radians(Pitch));
		front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));

		Front = glm::normalize(front);

		Right = glm::normalize(glm::cross(Front, WorldUp));
		Up = glm ::normalize(glm::cross(Right, Front));
	}
};

#endif
