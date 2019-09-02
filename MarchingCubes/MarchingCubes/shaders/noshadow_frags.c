#version 330

uniform float uKa, uKd, uKs;		// coefficients of each type of lighting
uniform vec3  uColor;			// object color
uniform vec3  uSpecularColor;		// light color
uniform float uShininess;		// specular exponent

in VS_OUT{
	
	vec3 vN;  //normal vector
	vec3 vL; // vector from point to light
    vec3 vE; // vector from point to eye
}vs;

void main() {
	
    vec3 Normal = normalize(vs.vN);
	vec3 Light  = normalize(vs.vL);
	vec3 Eye    = normalize(vs.vE);
	
	vec3 ambient = uKa * uColor;
	float d = max( dot(Normal,Light), 0. );   // only do diffuse if the light can see the point
	vec3 diffuse = uKd * d * uColor;
	
	float s = 0.;
	if( dot(Normal,Light) > 0. ){	          // only do specular if the light can see the point

	vec3 ref = normalize(  reflect( -Light, Normal )  );
	s = pow( max( dot(Eye,ref),0. ), uShininess );
	}
	vec3 specular = uKs * s * uSpecularColor;
	gl_FragColor = vec4( ambient + diffuse + specular,  1. );
	//gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0);
}