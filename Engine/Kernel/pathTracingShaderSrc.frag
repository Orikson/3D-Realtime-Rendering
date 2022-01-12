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

// ro - current position
// pos - center of rounded cylinder
// ra - radius of cylinder
// rb - radius of edge
// h - height of cylinder
float distTo_roundedCylinder(vec3 ro, vec3 pos, float ra, float rb, float h) {
  vec3 p = translate(ro, pos);
  vec2 d = vec2(length(p.xz)-2.0*ra+rb, abs(p.y) - h);
  return min(max(d.x,d.y),0.0) + length(max(d,0.0)) - rb;
}

// [COPIED FROM kernelf.cpp]
// data struct: (all unused are arbitrary)
//          (id #)
// [name]   [0]     [1]     [2]     [3]     [4]     [5]     [6]     [7]     [8]     [9]
// sphere   0       x       y       z       r                       red     green   blue    
// cone     1       x       y       z       c1      c2      h       red     green   blue
// box      2       x       y       z       w       l       h       red     green   blue
// r cyl	3		x		y		z		ra		rb		h       red     green   blue

// distance to closeset object in world (with an exclude)
// pos - current position
vec2 dist (vec3 pos, int exclude) {
	// dist to floor (y coord)
	float minD = MAXD;
    //float minD = pos.y+2;
    int curid = -1;

	for (int i = 0; i < size; i ++) {
        if (i != exclude) {
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

                case 3: // rounded cylinder
                    curD = distTo_roundedCylinder(
                        pos, 
                        vec3(
                            data[i*width+1], 
                            data[i*width+2], 
                            data[i*width+3]
                        ),
                        data[i*width+4],
                        data[i*width+5],
                        data[i*width+6]
                    );
                    break;
                
                default:// if something goes wrong i guess
                    break;
            }

            if (curD < minD) {
                minD = curD;
                curid = i;
            }
        }
	}

	return vec2(curid, minD);
}

vec3 getColor(int ind) {
    return vec3(data[ind*width+7], data[ind*width+8], data[ind*width+9]);
}

vec3 getN(vec3 p){
   float d = dist(p, -1).y;
   vec2 z = vec2(.001, 0.);	
   
   vec3 n = d - vec3(dist(p-z.xyy, -1).y, dist(p-z.yxy, -1).y, dist(p-z.yyx, -1).y);
   
   return normalize(n);
}

// returns vec2 representing (distance, object index)
// ro - ray origin
// rd - ray direction
vec2 raym (vec3 ro, vec3 rd, int exclude){
	float d = 0.;
   
   	for (int i = 0; i < 100; i ++){
       	vec3 p = ro + rd*d;
        vec2 res = dist(p, exclude);
        if (exclude == -2) { res.y *= -1.0; }
       	float dz = res.y;
       	d += dz;
       	if (d > 100.) {
	   		return vec2(d, -1); // void
       	}
		if (dz < 0.01) {
			return vec2(d-0.02+dz, res.x);
		}
   } return vec2(d, -1); // void
}

float getL(vec3 p){
   vec3 l = vec3(0, 6, 0);
   vec3 z = normalize(l-p);
   vec3 n = getN(p);
   
   float f = raym(p+n*0.2, z, -1).x; // raym in direction of light (if length < distance to light, then we didn't reach the light, otherwise we did)
   if (f < length(l-p)){
       return dot(n, z)*0.1; // shadow
   }else{
       return dot(n, z); // light
   }
}

vec3 reflect(vec3 v, vec3 n) {
    return v - 2*dot(v,n)*n;
}

bool refract2(vec3 v, vec3 n, float ni_over_nt, inout vec3 refracted) {
    vec3 uv = normalize(v);
    float dt = dot(uv, n);
    float disc = 1.0 - ni_over_nt * ni_over_nt * (1.0-dt*dt);
    if (disc > 0.0) {
        refracted = ni_over_nt * (uv - n*dt) - n*sqrt(disc);
        return true;
    } else {
        return false;
    }
}

float schlick(float csn, float idx) {
    float r0 = (1.0-idx) / (1.0+idx);
    r0 = r0*r0;
    return r0 + (1.0-r0)*pow(1.0-csn,5.0);
}

vec3 shade(vec3 p, int ind) {
    bool wu = fract(p.x*0.5)>0.5;
    bool wv = fract(p.z*0.5)>0.5;
    if (wu^^wv) {
        return getColor(ind);
    } else {
        return getColor(ind) + vec3(0., 0.5, 0.);
    }
}

int maxBounces = 5;
int samples = 1;

vec3 color(vec3 ro, vec3 rd) {
    vec3 attenuation = vec3(1.);
    int ignore = -1;
    for (int i = 0; i < maxBounces; i ++) {
        vec2 res = raym(ro, rd, ignore);

        if (int(res.y) == -1) { // void
            float t = rd.y;
            vec3 result = attenuation * ((1.0-t)*vec3(0.5,0.75,1.0)+t*vec3(0.25, 0.5, 1.0));
            return result;
        } else if (data[int(res.y)*width+10] == 0) { // matte (disperse)
            vec3 target = ro + res.x*normalize(rd) + getN(ro + res.x*normalize(rd)) + randInUnitSphere();
            ro = ro + res.x*normalize(rd) + getN(ro + res.x*normalize(rd))*0.02;
            rd = normalize(target-ro);
            
            vec3 curC;
            if (int(res.y) == 1) {
                curC = shade(ro + res.x*normalize(rd), 0);
            } else {
                curC = getColor(int(res.y));
            }
            attenuation *= 0.5*curC;//*color(ro2, rd2, c + 1);
        } else if (data[int(res.y)*width+10] == 1) { // metal (reflect)
            ro = ro + res.x*normalize(rd) + getN(ro + res.x*normalize(rd))*0.02;
            rd = reflect(rd, getN(ro));
        } else if (data[int(res.y)*width+10] == 2) { // glass (refract)
            vec3 outward_normal;
            float ni_over_nt;
            vec3 refracted;
            float reflect_prob = 1.0;
            float csn;
            float il = 1.0/length(rd);
            float drdnor = dot(rd, getN(ro + res.x*normalize(rd)));
            float idx = data[int(res.y)*width+11];
            float diff;
            if (drdnor > 0.0) { // inside going out
                outward_normal = getN(ro + res.x*normalize(rd));
                ni_over_nt = idx;
                csn = ni_over_nt * drdnor * il;
                ignore = -1;
            } else { // outside going in
                diff = 1;
                outward_normal = getN(ro + res.x*normalize(rd));
                ni_over_nt = 1.0/idx;
                csn = -ni_over_nt * drdnor * il;
                ignore = -2;
            }
            if (refract2(rd, outward_normal, ni_over_nt, refracted)) {
                reflect_prob = schlick(csn, idx);
            }
            if (randFloat() < reflect_prob) {
                ro = ro + res.x*normalize(rd);
                rd = reflect(rd, getN(ro));
            } else {
                ro = ro + res.x*normalize(rd) + diff * outward_normal * 0.04;
                rd = refracted;
            }
        }
    }

    float t = rd.y;
    vec3 result = attenuation * ((1.0-t)*vec3(0.5,0.75,1.0)+t*vec3(0.25, 0.5, 1.0));
    return result;
}

vec4 getC(vec2 uv, vec3 fd, vec3 upd, vec3 rtd, vec3 ro) {
	vec3 rd = normalize(fd+uv.x*rtd+uv.y*upd);

    return vec4(color(ro, rd), 1);
}

float hash12(vec2 p) {
	vec3 p3  = fract(vec3(p.xyx) * .1031);
    p3 += dot(p3, p3.yzx + 33.33);
    return fract((p3.x + p3.y) * p3.z);
}

void main() {
    float FOV = 0.5;
    vec2 uv = FOV*vec2(x, y);

    setSeed(uint(hash12(uv) * (y+1) * res.x * res.x + hash12(uv) * x));
    setSeed(randInt());// + uint(iTime*10000));

	//vec3 col = 0.5 + 0.5*cos(float(iFrame)/1000.0 + vec3(x, y, x) + vec3(0,2,4));
	//gl_FragColor = vec4(col, 1.0);

	//vec3 ro = vec3(5*cos(float(iFrame)/1000), 2, 5*sin(float(iFrame)/1000));
	vec3 ro = cPos;
	//vec3 fd = normalize(vec3(0,0,0)-ro);
	vec3 fd = normalize(cRot);
	vec3 upd = normalize(vec3(-fd.y*fd.xz,length(fd.xz)).xzy);
	vec3 rtd = normalize(cross(fd,upd));

	vec4 C = vec4(0);
    for (int i = 0; i < samples; i ++) {
        //C += getC(uv, fd, upd, rtd, ro);
        C += getC(uv+vec2( 1, 1)*0.25/(res/2.0), fd, upd, rtd, ro);
        C += getC(uv+vec2( 1,-1)*0.25/(res/2.0), fd, upd, rtd, ro);
        C += getC(uv+vec2(-1, 1)*0.25/(res/2.0), fd, upd, rtd, ro);
        C += getC(uv+vec2(-1,-1)*0.25/(res/2.0), fd, upd, rtd, ro);
    }
    //C += getC(uv+vec2( 1, 1)*0.25/(res/2.0), fd, upd, rtd, ro);
	//C += getC(uv+vec2( 1,-1)*0.25/(res/2.0), fd, upd, rtd, ro);
	//C += getC(uv+vec2(-1, 1)*0.25/(res/2.0), fd, upd, rtd, ro);
	//C += getC(uv+vec2(-1,-1)*0.25/(res/2.0), fd, upd, rtd, ro);

	//C /= 4.0;
	
	gl_FragColor = sqrt(C / samples / 4.0);

	//gl_FragColor = vec4(randInUnitSphere(), 1.0);
}