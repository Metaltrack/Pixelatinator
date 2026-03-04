#version 330 core

uniform float levels;
uniform bool bnw;
uniform vec2 u_resolution;
uniform sampler2D tex;
uniform float pixel_size;

void main(){
	vec2 uv = gl_FragCoord.xy / u_resolution.xy;

	vec2 coord = floor(gl_FragCoord.xy / pixel_size) * pixel_size;
    vec2 new_uv = coord / u_resolution.xy;

	vec4 color = texture(tex, new_uv);
	float greyscale = max(color.r, max(color.g, color.b));
	float avg = (color.r + color.g + color.b) / 3.0;
	float lower = floor(avg * levels) / levels;
	float lowerDiff = abs(avg - lower);
	float upper = ceil(avg * levels) / levels;
	float upperDiff = abs(upper - avg);

	float level = lowerDiff <= upperDiff ? lower : upper;
	float adjustment = level / avg;

	if(bnw){
		gl_FragColor = vec4(color.r * adjustment, color.r * adjustment, color.r * adjustment, 1.0);
	}else{
		gl_FragColor = vec4(color.rgb * adjustment, 1.0);
	}
}