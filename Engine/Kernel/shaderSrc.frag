#version 430

// float coordinates defined by vertex shader (between -1 and 1)
varying float x, y, z;
uniform int iFrame;
uniform float iTime;

const float MAXD = 100.0;

// layouts
layout (std430, binding=2) buffer shader_data
{ 
	vec2 res;
	int size;
	int width;
  	float data[];
};

// ro - current position
// pos - relative position
// returns - current position if pos is (0,0,0)
vec3 translate (vec3 ro, vec3 pos) {
	return ro - pos;
}


// distance functions adapted from https://www.iquilezles.org/www/articles/distfunctions/distfunctions.htm

// ro - current position
// pos - center of sphere
// r - radius of sphere
float distTo_sphere (vec3 ro, vec3 pos, float r) {
	vec3 p = translate(ro, pos);
	return length(p)-r;
}

// ro - current position
// pos - center of cone
// c - sin/cos of angle 
// h - height
float distTo_cone (vec3 ro, vec3 pos, vec2 c, float h) {
	vec3 p = translate(ro, pos);
	vec2 q = h*vec2(c.x/c.y,-1.0);
    
	vec2 w = vec2( length(p.xz), p.y );
	vec2 a = w - q*clamp( dot(w,q)/dot(q,q), 0.0, 1.0 );
	vec2 b = w - q*vec2( clamp( w.x/q.x, 0.0, 1.0 ), 1.0 );
	float k = sign( q.y );
	float d = min(dot( a, a ),dot(b, b));
	float s = max( k*(w.x*q.y-w.y*q.x),k*(w.y-q.y)  );
	return sqrt(d)*sign(s);
}

// ro - current position
// pos - center of box
// b - dimensions of box
float distTo_box (vec3 ro, vec3 pos, vec3 b) {
	vec3 p = translate(ro, pos);
	vec3 q = abs(p) - b;
  	return length(max(q,0.0)) + min(max(q.x,max(q.y,q.z)),0.0);
}

// [COPIED FROM kernelf.cpp]
// data struct: (all unused are arbitrary)
//          (id #)
// [name]   [0]     [1]     [2]     [3]     [4]     [5]     [6]     [7]     [8]
// sphere   0       x       y       z       r
// cone     1       x       y       z       c1      c2      h
// box      2       x       y       z       w       l       h

// distance to closeset object in world
// pos - current position
float dist (vec3 pos) {
	// dist to floor (y coord)
	float minD = pos.y+2;

	for (int i = 0; i < size; i ++) {
		float curD = minD;
		
		switch (int(data[i*width])) {
			case 0: // sphere
				curD = distTo_sphere(
					pos, 
					vec3(
						data[i*width+1], 
						data[i*width+2], 
						data[i*width+3]
					),
					data[i*width+4]
				);
				break;
			
			case 1: // cone
				curD = distTo_cone(
					pos, 
					vec3(
						data[i*width+1], 
						data[i*width+2], 
						data[i*width+3]
					),
					vec2(
						data[i*width+4],
						data[i*width+5]
					),
					data[i*width+6]
				);
				break;
			
			case 2: // rectangular prism
				curD = distTo_box(
					pos, 
					vec3(
						data[i*width+1], 
						data[i*width+2], 
						data[i*width+3]
					),
					vec3(
						data[i*width+4],
						data[i*width+5],
						data[i*width+6]
					)
				);
				break;
			
			default:// if something goes wrong i guess
				break;
		}

		if (curD < minD) {
			minD = curD;
		}
	}

	return minD;
}

// returns vec2 representing (distance, object index)
// ro - ray origin
// rd - ray direction
float raym (vec3 ro, vec3 rd){
	float d = 0.;
   
   	for (int i = 0; i < 100; i ++){
       	vec3 p = ro + rd*d;
       	float dz = dist(p);
       	d += dz;
       	if (d > 100.) {
	   		return d;
       	}
		if (dz < 0.01) {
			return d-0.02+dz;
		}
   } return d;
}

vec3 getN(vec3 p){
   float d = dist(p);
   vec2 z = vec2(.01, 0.);	
   
   vec3 n = d - vec3(dist(p-z.xyy), dist(p-z.yxy), dist(p-z.yyx));
   
   return normalize(n);
}

float getL(vec3 p){
   //vec3 l = vec3(0. + 5.*cos(iTime*1.), 6., 3. + 5.*sin(iTime*1.));
   vec3 l = vec3(0, 6, 0);
   vec3 z = normalize(l-p);
   vec3 n = getN(p);
   
   float f = raym(p+n*0.2, z);
   if (f < length(l-p)){
       return dot(n, z)*0.1;
   }else{
       return dot(n, z);
   }
}

vec4 getC(vec2 uv, vec3 fd, vec3 upd, vec3 rtd, vec3 ro) {
	vec3 rd = normalize(fd+uv.x*rtd+uv.y*upd);
	
	float d = raym(ro, rd);
  
	vec3 p = ro + rd * d;
	
	float l = getL(p);

	return vec4(0, l, l, 1);
}

void main() {
	//vec3 col = 0.5 + 0.5*cos(float(iFrame)/1000.0 + vec3(x, y, x) + vec3(0,2,4));
	//gl_FragColor = vec4(col, 1.0);

	vec2 uv = vec2(x, y);
	vec3 ro = vec3(5*cos(float(iFrame)/1000), 2, 5*sin(float(iFrame)/1000));
	vec3 fd = normalize(vec3(0,0,0)-ro);
	vec3 upd = vec3(-fd.y*fd.xz,length(fd.xz)).xzy;
	vec3 rtd = cross(fd,upd);
	
	vec4 C = vec4(0);

	C += getC(uv+vec2( 1, 1)*0.25/(res/2.0), fd, upd, rtd, ro);
	C += getC(uv+vec2( 1,-1)*0.25/(res/2.0), fd, upd, rtd, ro);
	C += getC(uv+vec2(-1, 1)*0.25/(res/2.0), fd, upd, rtd, ro);
	C += getC(uv+vec2(-1,-1)*0.25/(res/2.0), fd, upd, rtd, ro);

	C /= 4.0;
	
	gl_FragColor = C;
}