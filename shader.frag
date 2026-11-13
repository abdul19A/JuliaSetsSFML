#version 400

uniform vec2 u_resolution;
uniform vec2 u_seed;
uniform vec2 e_offsetX;
uniform vec2 e_offsetY;
uniform vec2 e_zoomFactor;
uniform int numIterations;
uniform float u_time;

vec2 normRes = normalize(u_resolution);

// get ALL position offsets, multiply them by the basezoomfactor to the power of times (int) then add them together
double doubleEmulate(vec2 flt){
	return double(flt.x) + double(flt.y);
}

// emulated double variables
dvec2 offset = dvec2(doubleEmulate(e_offsetX), -doubleEmulate(e_offsetY));
double zoomFactor = doubleEmulate(e_zoomFactor);

// sqaure of a complex number represented as a vec2 (a+bi)^2 = (a^2 - b^2) + 2abi
dvec2 complexSquare(dvec2 z) {
	return dvec2(z.x * z.x - z.y * z.y, 2.0 * z.x * z.y);
}
// radius of a complex number represented as a vec2
float getRadius(vec2 z) {
	return sqrt(z.x * z.x + z.y * z.y);
}
// mandelbrot calculation, returns number of iterations
int mandelbrot(dvec2 c) {	
	dvec2 k = u_seed;
	for (int i = 0; i < numIterations; i++){
		k = complexSquare(k) + c;
		if (dot(k, k) > 4) {
			return i;
		}
	}
	return 0;
}
// coloring
//cool rings
vec4 getColor3(int i){
	if (i==0){
	return vec4(1,0,0,1);
	}
	return vec4((i%2+0.1)/2, (i%3/2)/2, i%5/4, 1);
}
vec4 getColor2(int i){
	if (i==0){
	return vec4(0,0,0,1);
	}
	return vec4((i%2+0.1)/2, (i%3/2+0.5)/2, i%5/4, 1);
}
// smooth gradient coloring
vec4 getColor(int i){
	float t = (i+u_time)/numIterations;
	return vec4(sqrt(t),(1-t)*(1-t), log(1/t)/log(2), 4);
}
// terrible chatgpt coloring
vec4 getColor1(int i){
	float t = i/numIterations;
	return vec4(
		0.5 + 0.5 * cos(3.0 + t * 5.0),
		0.5 + 0.5 * cos(3.0 + t * 5.0 + 2.0),
		0.5 + 0.5 * cos(3.0 + t * 5.0 + 4.0),
		1.0
	);
}
// mainloop
void main(){
	dvec2 uv = gl_FragCoord.xy / u_resolution - vec2(0.5, 0.5); // convert to normalized device coordinates centered at (0,0)
	dvec2 off = offset / u_resolution; // normalize offset to resolution
	dvec2 pt = uv * zoomFactor - off; // apply zoom and offset to the point
	pt *= normRes; // adjust for aspect ratio based on normalized resolution
	int mb = mandelbrot(pt); 
	gl_FragColor = getColor(mb);
}