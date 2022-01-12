#ifndef CAMERA_H
#define CAMERA_H

#include <SDL.h>
#include "../Math/Math.hpp"

class Camera{
	public:
		Camera(vec3<float> from, vec3<float> to, float theta, float near, float far);
		void keyboardEvent(SDL_Event* event);
		void mouseEvent(SDL_Event* event);
		void windowEvent(SDL_Event* event);
		void updateMatrices();
		void updateCoordinates();
		void updateCamera();
		void setFovy(float theta);
		void setPosition(vec3<float> coordinate);
		mat4<float>& getViewMatrix();
		mat4<float>& getProjectionMatrix();
		mat4<float>& getViewAndProjectionMatrix();
		vec3<float>& getPosition();
		vec3<float>& getTarget();
		float getFovy() const;
		float getYaw() const;
		float getPitch() const;		
	private:
		//Matrices
		mat4<float> view;
		mat4<float> projection;
		mat4<float> viewProjection;

		//Camera control angle view
		float yaw;
		float pitch;
		float fovy;
		float sensitivity;
		float zNear;
		float zFar;

		//Camera control offset
		vec3<float> position;
		vec3<float> target;
		bool RIGHT = false;
		bool FOWARD = false;
		bool LEFT = false;
		bool BACKFOWARD = false;
		float speed;

		//Frame time controls
		float lastFrameTime;
		float currentFrameTime;
		float deltaTime;
	};
#endif
