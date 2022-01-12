#include "..\Common\Common.hpp"
#include "Camera.hpp"

Camera::Camera(vec3<float> from, vec3<float> to, float theta, float near, float far){
	zNear 		   = near;
	zFar  		   = far;
	yaw 	 	   = -90.0f;
	pitch    	   = 0.0f;
	sensitivity    = 0.10f;
	speed   	   = 0.01f;
	fovy           = theta;
	position       = from;
	target         = to;
	view           = lookAt(position, position + target, vec3<float>(0.0f, 1.0f, 0.0f));
	projection     = perspective(fovy, (float)ext_screen_width / (float)ext_screen_height, zNear, zFar);
	viewProjection = view * projection;
}

bool on = false;
void Camera::keyboardEvent(SDL_Event* event){
	if(event->type == SDL_KEYDOWN && event->key.repeat == 0){
		switch(event->key.keysym.scancode){
			case SDL_SCANCODE_D:
				RIGHT = true;
				break;
			case SDL_SCANCODE_A:
				LEFT = true;
				break;
			case SDL_SCANCODE_W:
				FOWARD = true;
				break;
			case SDL_SCANCODE_S:
				BACKFOWARD = true;
				break;
			default:
				break;
		}
	}
	else if(event->type == SDL_KEYUP && event->key.repeat == 0){
		switch(event->key.keysym.scancode){
			case SDL_SCANCODE_D:
				RIGHT = false;
				break;
			case SDL_SCANCODE_A:
				LEFT = false;
				break;
			case SDL_SCANCODE_W:
				FOWARD = false;
				break;
			case SDL_SCANCODE_S:
				BACKFOWARD = false;
				break;
			default:
				break;
		}
	}
}

void Camera::mouseEvent(SDL_Event* event){
	if(event->type == SDL_MOUSEMOTION){
		yaw   += event->motion.xrel * sensitivity;
		pitch -= event->motion.yrel * sensitivity;
		if(pitch > 89.0f){
			pitch = 89.0f;
		}
		if(pitch < -89.0f){
			pitch = -89.0f;
		}
		updateCamera();
	}
}

void Camera::windowEvent(SDL_Event* event){
	if(event->type == SDL_WINDOWEVENT){
		if(event->window.event == SDL_WINDOWEVENT_SIZE_CHANGED){
			ext_screen_width  = event->window.data1;
			ext_screen_height = event->window.data2;
			projection        = perspective(fovy, (float)ext_screen_width / (float)ext_screen_height, zNear, zFar);
			glViewport(0, 0, ext_screen_width, ext_screen_height); 
		}
	}
}

void Camera::updateMatrices(){
	view 		   = lookAt(position, position + target, vec3<float>(0.0f, 1.0f, 0.0f));
	viewProjection = view * projection;
}

void Camera::updateCoordinates(){
	currentFrameTime = SDL_GetTicks();
	deltaTime        = currentFrameTime - lastFrameTime;
	if(RIGHT)
		position = position + (normalize(cross(target, vec3<float>(0.0f, 1.0f, 0.0f)))) * (speed * deltaTime);
	if(LEFT)
		position = position - (normalize(cross(target, vec3<float>(0.0f, 1.0f, 0.0f)))) * (speed * deltaTime);
	if(FOWARD)
		position = position + target * (speed * deltaTime);
	if(BACKFOWARD)
		position = position - target * (speed * deltaTime);
	lastFrameTime = currentFrameTime;
	updateMatrices();
}

void Camera::updateCamera(){
	target.x = cos(radians(yaw)) * cos(radians(pitch));
	target.y = sin(radians(pitch));
	target.z = sin(radians(yaw)) * cos(radians(pitch));
}

void Camera::setFovy(float theta){
	fovy = theta;
	updateMatrices();
}

void Camera::setPosition(vec3f coordinate){
	position = coordinate;
	updateMatrices();
}

mat4<float>& Camera::getViewAndProjectionMatrix(){
	return viewProjection;
}

mat4<float>& Camera::getViewMatrix(){
	return view;
}

mat4<float>& Camera::getProjectionMatrix(){
	return projection;
}

vec3<float>& Camera::getPosition(){
	return position;
}

vec3<float>& Camera::getTarget(){
	return target;
}

float Camera::getFovy() const{
	return fovy;
}

float Camera::getYaw() const{
	return yaw;
}

float Camera::getPitch() const{
	return pitch;
}