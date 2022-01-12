#include "../headers/Math.hpp"
#include "../headers/Material.hpp"
#include "../headers/Light.hpp"
#include "../headers/Text.hpp"

int main(){
	vec4f  position(20.0f, 0.0f, 10.0f);
	vec3f  normal(0.0f, 0.0f, -1.0f);
	vec2uchar uvcoord(255, 255);

	position.show("position");
	normal.show("normal");
	uvcoord.show("uvcoord");

	position = position * translate(vec3f(0.0f, 1.0f, 2.0f));
	position = position * rotateX(45.0f);
	position = position * rotateY(45.0f);
	position = position * rotateZ(45.0f);
	position = position * scale(vec3f(2.0f, 2.0f, 2.0f));
	vec3f orientation = normalize(vec3f(position.x, position.y, position.z));
	orientation.show("transformed");

	Light Sun;
	Sun.show();
	Material Surface;
	Surface.show();

	return 0;
}

/*
media de 1 milesimo

*/