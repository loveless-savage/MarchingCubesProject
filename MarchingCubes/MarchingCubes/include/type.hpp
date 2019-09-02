#ifndef TYPE_H_
#define TYPE_H_
#include <glm/glm.hpp>

enum class Light_Type {

	SPOT,
	POINT,
	DIRECTION
};
// 3D coordinates class
struct XYZ { float x, y, z; };
// color class
struct RGB { float r, g, b; };
// combine the previous two
struct XYZRGB { XYZ pos; RGB color; };

// light source
struct Light {
	// store the type of this source as a Light_Type enum
	Light_Type light_type;
	glm::vec3 color;
	glm::vec3 pos;

	// initialize with empty constructor
	Light() {}

	// initialize with color, coordinates, and type of source
	Light(float r, float g, float b,
		float x, float y, float z,
		Light_Type in_light_type = Light_Type::DIRECTION) {

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
