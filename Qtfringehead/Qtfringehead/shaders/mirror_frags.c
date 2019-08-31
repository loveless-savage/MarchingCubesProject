#version 330

uniform mat4 model_view_proj;
uniform vec3 mirror_pos;

uniform sampler2D mirrorTexture; // texture computed by render_mirror_tex_frags.c
uniform int width;
uniform int height;

in vec3 vColor;


void main() {
	// find screen coordinate of mirror's center
	vec4 mirror_screen_loc = model_view_proj * vec4(mirror_pos,1);
	vec3 mirror_offset = mirror_screen_loc.xyz/mirror_screen_loc.w;
	mirror_offset /= 0.5;
	mirror_offset += 0.5;

	// find appropriate mirror texture coordinate
	vec4 uColor = texture( mirrorTexture,
		// find normalized screen coordinates
		vec2( 1-gl_FragCoord.x/width, gl_FragCoord.y/height+mirror_offset.y-0.5 )
	);
	// black background reflected in mirror
	if(uColor==vec4(0,0,0.2,1)) uColor=vec4(0,0,0,1);

	// horizontal stripes indicating uniform values
	if(abs(gl_FragCoord.y/height-mirror_offset.y)<0.005) uColor=vec4(1,0,0,1);
	if(abs(gl_FragCoord.y/height-0.5)<0.005) uColor=vec4(0,1,0,1);

	// output final value to screen buffer
	gl_FragColor = uColor;
}
