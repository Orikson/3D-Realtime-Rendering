#version 430

// float coordinates defined by vertex shader (between -1 and 1)
varying float x, y, z;
uniform float iFrame;

// layouts
layout (std430, binding=2) buffer shader_data
{ 
  float data[];
};

// pos - sphere position
// r - radius
// ro - point to compare
float distTo_Sphere (vec3 pos, float r, vec3 ro) {
	return 1.0;
}


void main() {
	//vec3 col = 0.5 + 0.5*cos(iFrame/100 + vec3(x, y, x) + vec3(0,2,4));
	//gl_FragColor = vec4(col, 1.0);

	gl_FragColor = vec4(data[0], 0., abs(x), 1.0);
}