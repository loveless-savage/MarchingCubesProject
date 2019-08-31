#version 330

uniform vec3  uK;		// coefficients of each type of lighting uKa, uKd, uKs
uniform vec3  uColor;			// object color
uniform bool  isColorful;		// are the vertices multicolored?
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
	vec3 surfaceColor; // color extracted from interpolation
	vec3 vN;  //normal vector
	vec3 vL; // vector from point to light
	vec3 vL2;
    vec3 vE; // vector from point to eye
	vec4 shadowCoord; //shadow map coordinate
	vec4 shadowCoord2;
}vs;


layout (location=0) out vec3 color; // render to texture, not screen


void main() {
	// if entire model has one color
	vec3 dColor = uColor;
	// if color is determined via vertex interpolation
	if(isColorful) dColor = vs.surfaceColor;
	
	vec3 Normal = normalize(vs.vN);
	vec3 Eye    = normalize(vs.vE);
	vec3 Light  = normalize(vs.vL);
	vec3 Light2  = normalize(vs.vL2);
	// spotlight width- 1.00 is zero, 0.00 is infinitely wide
	if(0 == light_type){
	    if(dot(-normalize(lightPosition), -Light) < 0.93)
		    Light = vec3(0.0);
	}
	if(0 == light_type2){
	    if(dot(-normalize(lightPosition2), -Light2) < 0.93)
		    Light2 = vec3(0.0);
	}

	// use w coordinate to apply projection transform
	vec3 projCoords = vs.shadowCoord.xyz / vs.shadowCoord.w;
	vec3 projCoords2 = vs.shadowCoord2.xyz / vs.shadowCoord2.w;
	// map from [-1.,1.] to [0.,1.]
	projCoords = projCoords * 0.5 + 0.5;
	projCoords2 = projCoords2 * 0.5 + 0.5;
	
	// shadow beats lighting if distance is equal
	float bias = max(0.05 * (1.0 - dot(Normal, Light)), 0.005) / vs.shadowCoord.w; 
	float bias2 = max(0.05 * (1.0 - dot(Normal, Light2)), 0.005) / vs.shadowCoord2.w;
	
	// is fragment in shadow? 0.0 = shaded from direct light
	float visibility, visibility2;
	// use shadow maps to see if current fragment is shaded
    if ((projCoords.z - bias) > texture( shadowMap, projCoords.xy ).z )
        visibility = 0.0;
    else
		visibility = 1.0;
	 if ((projCoords2.z - bias2) > texture( shadowMap2, projCoords2.xy ).z )
        visibility2 = 0.0;
    else
		visibility2 = 1.0;

	
	// ambient light is constant, only dependent on surface color
	vec3 ambient = uK.x * dColor;
	
	// angle of light hitting surfaces- negative dot fragments are "culled" out
	float d = visibility * max( dot(Normal,Light), 0. );  
    float d2 = 	visibility2 * max( dot(Normal,Light2), 0. );
	// finally get diffuse: combine surface/light color, light angle & reflectivity
	vec3 diffuse = uK.y * d * dColor * lightColor + uK.y * d2 * dColor * lightColor2;
	
	// how close is the light/frag vector to a reflected frag/camera vector?
	float s = 0., s2 = 0.;
	if( d > 0. ){ // "culling" test applies to specular too
	  vec3 ref = normalize(  reflect( -Light, Normal )  );
	  s =  visibility * pow( max( dot(Eye,ref),0. ), uShininess );
	}
	if( d2 > 0. ){
	  vec3 ref = normalize(  reflect( -Light2, Normal )  );
	  s2 = visibility2 * pow( max( dot(Eye,ref),0. ), uShininess );
	}
	
	vec3 specular = uK.z * s * uSpecularColor * lightColor + uK.z * s2 * uSpecularColor * lightColor2;
	
	color = vec3( ambient +  diffuse + specular );
	//gl_FragColor = vec4(vec3(texture( shadowMap, projCoords.xy ).z),1.0);
	
}
