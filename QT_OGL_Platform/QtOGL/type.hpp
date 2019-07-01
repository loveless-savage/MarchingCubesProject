#ifndef TYPE_H_
#define TYPE_H_
#include <glm/glm.hpp>

enum class Light_Type{

	SPOT,
	POINT,
	DIRECTION
};
struct XYZ{ float x, y, z; };
struct RGB{ float r, g, b; };
struct XYZRGB{ XYZ pos; RGB color; };
struct Light{

	Light_Type light_type;
	glm::vec3 color;
	glm::vec3 pos;
	Light(){}
	Light(float r, float g, float b, 
		  float x, float y, float z, 
		  Light_Type in_light_type = Light_Type::DIRECTION){

		color.r = r;
		color.g = g;
		color.b = b;

		pos.x = x;
		pos.y = y;
		pos.z = z;

		light_type = in_light_type;
	}
};


#endif