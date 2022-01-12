#version 430

// float coordinates defined by vertex shader (between -1 and 1)
varying float x, y, z;
uniform int iFrame;
uniform float iTime;
uniform vec3 cPos;
uniform vec3 cRot;

const float MAXD = 100.0;

// layouts
layout (std430, binding=2) buffer shader_data
{ 
	vec2 res;
	int size;
	int width;
	float data[];
};

uint rngstate = uint(1);

uint randInt() {
    rngstate ^= rngstate << 13;
    rngstate ^= rngstate >> 17;
	rngstate ^= rngstate << 5;
    return rngstate;
}

void setSeed(uint seed) {
    rngstate = seed;
    randInt(); randInt(); randInt(); // Shift some bits around
}

float randFloat() {
    randInt(); randInt(); randInt(); // Shift bits ;D
    return fract(float(randInt()) / float(1<<32 - 5));
}

vec3 randDir(vec3 n) {
    float u = randFloat();
    float v = randFloat();

    float theta = 2.0*3.1415*u;
    float phi = acos(2.0*v-1.0);
    
    vec3 res = vec3(sin(phi)*cos(theta), sin(phi)*sin(theta), cos(phi));
    
    return sign(dot(n, res))*res;
}
vec3 randDir() {
    float u = randFloat();
    float v = randFloat();

    float theta = 2.0*3.1415*u;
    float phi = acos(2.0*v-1.0);
    
    return vec3(sin(phi)*cos(theta), sin(phi)*sin(theta), cos(phi));
}
vec3 randInUnitSphere() {
    vec3 d = randDir();
    float m = randFloat();

    return d*m;
}

struct ray {
    vec3 ro;    // ray origin
    vec3 rd;    // ray direction
};

struct collision {
    float t;    // time to collision
    vec3 p;     // position of collision
    vec3 n;     // normal at collision
    vec3 obc;   // object center
    int obi;    // object index
};

// all collide functions return the point where the ray intersects the simple object

// ray, sphere position/radius/orientation, collision
vec3 collide_sphere(ray ray, vec3 pos, float r, vec3 rot, inout collision col) {
    return vec3(1);
}


// ray, box position/dimensions/orientation, collision
vec3 collide_box(ray ray, vec3 pos, vec3 b, vec3 rot, inout collision col) {
    return vec3(1);
}

void main() {
    gl_FragColor = vec4(0., 0., 0., 1.);
}