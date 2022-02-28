#ifndef CAMERA_H
#define CAMERA_H

#include "../Math/Math.hpp"

// - SDL2
#define SDL_MAIN_HANDLED
#include "SDL2/SDL.h"

// - Standart header files
#include <functional>

class Camera{
	public:
		// - General functions
		Camera(vec3<float> from, vec3<float> to, float theta, float near, float far);
		void keyboardEvent(SDL_Event* event);
		void mouseEvent(SDL_Event* event);
		void windowEvent(SDL_Event* event);
		void updateMatrices();
		void updateCoordinates();
		void updateCamera();
		
		// - Setters
		void setFovy(float theta);
		void setPosition(vec3<float> coordinate);
		void setSpeed(float value);
		void setSensitivity(float value);
		
		// - Getters
		mat4<float>& getViewMatrix();
		mat4<float>& getProjectionMatrix();
		mat4<float>& getViewAndProjectionMatrix();
		vec3<float>& getPosition();
		vec3<float>& getTarget();
		float getFovy() const;
		float& getYaw();
		float& getPitch();		
		float getSpeed() const;
		float getSensitivity() const;	
		float getFar() const;
		float getNear() const;

		bool& isMovingRight(){
			return RIGHT;
		}

		bool& isMovingLeft(){
			return LEFT;
		}

		bool& isMovingFoward(){
			return FOWARD;
		}

		bool& isMovingBackfoward(){
			return BACKFOWARD;
		}
	private:
		// - Matrices
		mat4<float> view;
		mat4<float> projection;
		mat4<float> viewProjection;

		// - Camera control angle view
		float yaw;
		float pitch;
		float fovy;
		float sensitivity;
		float zNear;
		float zFar;

		// - Camera control offset
		vec3<float> position;
		vec3<float> target;
		bool RIGHT;
		bool FOWARD;
		bool LEFT;
		bool BACKFOWARD;
		bool COLLISION_DETECTION;
		float speed;

		// - Frame time controls
		float lastFrameTime;
		float currentFrameTime;
		float deltaTime;		
	};
#endif
