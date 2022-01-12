#include "../headers/world/Light.hpp"
#include "../headers/extern/COMMON.hpp"

Light::Light(){
	setName("UNKNOWN");
	setPosition(0.0f, 0.0f, 0.0f);
	setAmbient(0.0f, 0.0f, 0.0f);
	setDiffuse(0.0f, 0.0f, 0.0f);
	setSpecular(0.0f, 0.0f, 0.0f);
	setDirection(0.0f, 0.0f, 0.0f);
	setConstantValue(0.0f);
	setLinearValue(0.0f);
	setQuadraticValue(0.0f);
}

Light::Light(const char* n, vec3f direction, vec3f position, vec3f ambient, vec3f diffuse, vec3f specular,  
			 float constant, float linear, float quadratic, float cosInner, float cosOuter, bool spot){
	setName(n);
	setDirection(direction.x, direction.y, direction.z);
	setPosition(position.x, position.y, position.z);
	setAmbient(ambient.x, ambient.y, ambient.z);
	setDiffuse(diffuse.x, diffuse.y, diffuse.z);
	setSpecular(specular.x, specular.y, specular.z);
	setConstantValue(constant);
	setLinearValue(linear);
	setQuadraticValue(quadratic);
	setCosInner(cosInner);
	setCosOuter(cosOuter);
	setType(spot);
	float compMax = std::fmaxf(std::fmaxf(diffuse.r, diffuse.g), diffuse.b);
	float delta   = linear * linear - 4.0f * quadratic * (constant - (256.0f / 5.0f) * compMax);  
	radius 		  = (-linear + sqrtf(delta)) / (2.0f * quadratic); 
}

void Light::setName(const char* n){
	name = n;
}

void Light::setPosition(float x, float y, float z){
	position.x = x;
	position.y = y;
	position.z = z;
}

void Light::setDirection(float x, float y, float z){
	direction.x = x;
	direction.y = y;
	direction.z = z;
}

void Light::setAmbient(float red, float green, float blue){
	ambient.x = red;
	ambient.y = green;
	ambient.z = blue;
}

void Light::setDiffuse(float red, float green, float blue){
	diffuse.x = red;
	diffuse.y = green;
	diffuse.z = blue;
}

void Light::setSpecular(float red, float green, float blue){
	specular.x = red;
	specular.y = green;
	specular.z = blue;
}

void Light::setConstantValue(float kc){
	this->constant = kc;
}

void Light::setLinearValue(float ki){
	this->linear = ki;
}

void Light::setQuadraticValue(float kq){
	this->quadratic = kq;
}

void Light::setCosInner(float cos){
	this->cosInner = cos;
}

void Light::setCosOuter(float cos){
	this->cosOuter = cos;
}

void Light::setType(bool flag){
	this->type = flag;
}

void Light::setOffset(vec3f center, vec3f angle, vec3f radius, vec3f speed){
	float x = (center.x) + cos(radians(angle.x) * speed.x) * radius.x;
	float y = (center.y) + sin(radians(angle.y) * speed.y) * radius.y;
	float z = (center.z) + sin(radians(angle.z) * speed.z) * radius.z;
	setPosition(x, y, z);
}

const char* Light::getName() const{
	return name;
}

vec3f& Light::getPosition(){
	return position;
}

vec3f& Light::getAmbient(){
	return ambient;
}

vec3f& Light::getDiffuse(){
	return diffuse;
}

vec3f& Light::getSpecular(){
	return specular;
}

vec3f& Light::getDirection(){
	return direction;
}

float Light::getRadius() const{
	return radius;
}

float Light::getConstantValue() const{
	return constant;
}

float Light::getLinearValue() const{
	return linear;
}

float Light::getQuadraticValue() const{
	return quadratic;
}

float Light::getCosInner() const{
	return cosInner;
}

float Light::getCosOuter() const{
	return cosOuter;
}

bool Light::getType() const{
	return type;
}

void Light::show(){
	std::cout << std::setw(10) << "Light Name: " << getName() << std::endl;
	getPosition().show("Position");
	getDirection().show("Direction");
	getAmbient().show("Ambient");
	getDiffuse().show("Diffuse");
	getSpecular().show("Specular");
	std::cout << std::setw(12)  << "Radius: ";
	std::cout << std::setw(12)  << radius << std::endl;
	std::cout << std::setw(12)  << "Constant: "; 
	std::cout << std::setw(10)  << constant  << std::endl;
	std::cout << std::setw(12)  << "Linear: ";
	std::cout << std::setw(10)  << linear	<< std::endl;
	std::cout << std::setw(12)  << "Quadratic: ";
	std::cout << std::setw(10)  << quadratic << std::endl;
	std::cout << std::setw(12)  << "Cos Inner: ";
	std::cout << std::setw(10)  << cosInner << std::endl;
	std::cout << std::setw(12)  << "Cos Outer: ";
	std::cout << std::setw(10)  << cosOuter << std::endl;
}