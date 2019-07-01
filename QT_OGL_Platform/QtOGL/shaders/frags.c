#version 330

uniform vec3  uK;		// coefficients of each type of lighting uKa, uKd, uKs
uniform vec3  uColor;			// object color
uniform vec3  uSpecularColor;		// light color
uniform float uShininess;		// specular exponent
uniform sampler2D shadowMap;
uniform sampler2D shadowMap2;
uniform vec3 lightColor;
uniform vec3 lightColor2;
uniform vec3 lightPosition;
uniform vec3 lightPosition2;
uniform int light_type;
uniform int light_type2;



in VS_OUT{
	
	vec3 vN;  //normal vector
	vec3 vL; // vector from point to light
	vec3 vL2;
    vec3 vE; // vector from point to eye
	vec4 shadowCoord; //shadow map coordinate
	vec4 shadowCoord2;
}vs;

void main() {
	
	vec3 Normal = normalize(vs.vN);
	vec3 Eye    = normalize(vs.vE);
	vec3 Light  = normalize(vs.vL);
	vec3 Light2  = normalize(vs.vL2);
	if(0 == light_type){
	    if(dot(-normalize(lightPosition), -Light) < 0.93)
		    Light = vec3(0.0);
	}
	if(0 == light_type2){
	    if(dot(-normalize(lightPosition2), -Light2) < 0.93)
		    Light2 = vec3(0.0);
	}
	//shadow map
	float visibility, visibility2;
	vec3 projCoords = vs.shadowCoord.xyz / vs.shadowCoord.w;
	vec3 projCoords2 = vs.shadowCoord2.xyz / vs.shadowCoord2.w;
	
	projCoords = projCoords * 0.5 + 0.5;
	projCoords2 = projCoords2 * 0.5 + 0.5;
	
	float bias = max(0.05 * (1.0 - dot(Normal, Light)), 0.005) / vs.shadowCoord.w;  
	float bias2 = max(0.05 * (1.0 - dot(Normal, Light2)), 0.005) / vs.shadowCoord2.w; 
	
    if ((projCoords.z - bias) > texture( shadowMap, projCoords.xy ).z )
        visibility = 0.0;
    else
		visibility = 1.0;
	
	 if ((projCoords2.z - bias2) > texture( shadowMap2, projCoords2.xy ).z )
        visibility2 = 0.0;
    else
		visibility2 = 1.0;

	
	
	vec3 ambient = uK.x * uColor;
	float d = visibility * max( dot(Normal,Light), 0. );  
    float d2 = 	visibility2 * max( dot(Normal,Light2), 0. ); // only do diffuse if the light can see the point
	vec3 diffuse = uK.y * d * uColor * lightColor + uK.y * d2 * uColor * lightColor2;
	
	float s = 0.;
	float s2 = 0.;
	if( d > 0. ){	          // only do specular if the light can see the point

	  vec3 ref = normalize(  reflect( -Light, Normal )  );
	  s =  visibility * pow( max( dot(Eye,ref),0. ), uShininess );
	
	}
	if( d2 > 0. ){	          // only do specular if the light can see the point
	  
	  vec3 ref = normalize(  reflect( -Light2, Normal )  );
	  s2 = visibility2 * pow( max( dot(Eye,ref),0. ), uShininess );
	}
	
	vec3 specular = uK.z * s * uSpecularColor * lightColor + uK.z * s2 * uSpecularColor * lightColor2;
	
	gl_FragColor = vec4( ambient +  diffuse + specular   ,  1. );
	//gl_FragColor = vec4(vec3(texture( shadowMap, projCoords.xy ).z),1.0);
	
}