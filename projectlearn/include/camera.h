#ifndef CAMERA_H
#define CAMERA_H

#include<glad/glad.h>
#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include "glm/ext/matrix_transform.hpp"
#include "glm/fwd.hpp"

#include <math.h>
#include<vector>

//define possible camera options
enum Camera_Movement
{
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT,
	UP,
	DOWN,
	//circular movement
	CRIGHT,
	CLEFT
};

//default camera values

const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 7.5f;
const float SENSITIVITY = 0.05f;
const float ZOOM = 45.0f;
const float EULERFACTOR = 30.f;

class Camera 
{
	public:
		//camera attributes
		glm::vec3 Position;
		glm::vec3 Front;
		glm::vec3 Up;
		glm::vec3 Right;
		glm::vec3 WorldUp;
		bool otherlookat = false;

		//euler angles;
		float Yaw;
		float Pitch;

		//camera options

		float MovementSpeed;
		float MouseSensitivity;
		float Zoom;

		Camera(glm::vec3 position=glm::vec3(0.0f, 0.0f, -35.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f),float yaw = YAW, float pitch= PITCH) : MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
	    {
			Front = glm::vec3(0.0f,0.0f,35.f);
			Position = position;
			WorldUp = up;
			Yaw = yaw;
			Pitch = PITCH;
			updateCameraVectors();
		}

		glm::mat4 GetViewMatrix()
		{
			// std::cerr << (Position+Front).x <<  " " << (Position+Front).y << " " << (Position+Front).z << std::endl;
			return glm::lookAt(Position, Position+Front, Up);
			// return glm::lookAt(Position,glm::vec3(0,0,0), Up);
		}

		void ProcessKeyboard(Camera_Movement direction, float deltaTime)
		{
			float velocity = MovementSpeed * deltaTime;
			if(direction == FORWARD)
				Position += Front * velocity ;
			if(direction == BACKWARD)
				Position -= Front * velocity;
			if(direction == LEFT)
				Position -= Right * velocity;
			if(direction == RIGHT)
				Position += Right * velocity ;
			if(direction == CLEFT) {
				Yaw -= deltaTime*EULERFACTOR;
				updateCameraVectors();
			}
			if(direction == CRIGHT) {
				
				Yaw += deltaTime*EULERFACTOR;
				updateCameraVectors();
			}
			if(direction == UP) {
				Position.y += velocity;
				//Pitch += deltaTime*EULERFACTOR;
				//updateCameraVectors();
			}
			if(direction == DOWN) {
				Position.y -= velocity;
				//Pitch -= deltaTime*EULERFACTOR;
				//updateCameraVectors();
			}
		}

		void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true)
		{
			xoffset *= MouseSensitivity;
			yoffset *= MouseSensitivity;

			Yaw += xoffset ;
			Pitch += yoffset;

			if(constrainPitch)
			{
				if(Pitch > 89.0f)
					Pitch = 89.0f;
				if(Pitch < -89.0f)
					Pitch = -89.0f;
			}
			updateCameraVectors();
		}

		void ProcessMouseScroll(float yoffset)
		{
			Zoom -= (float) yoffset;

			if(Zoom<1.0f)
				Zoom = 1.0f;
			if(Zoom > 45.0f)
				Zoom = 45.0f;
		}

private:
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
