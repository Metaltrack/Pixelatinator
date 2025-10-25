#version 330 core

uniform float levels;
uniform bool racist_filter;
uniform vec2 u_resolution;
uniform sampler2D tex;

void main(){
	vec2 uv = gl_FragCoord.xy / u_resolution.xy;

	vec4 color = texture(tex, uv);
	float greyscale = max(color.r, max(color.g, color.b));

	float lower = floor(greyscale * levels) / levels;
	float lowerDiff = abs(greyscale - lower);
	float upper = ceil(greyscale * levels) / levels;
	float upperDiff = abs(upper - greyscale);

	float level = lowerDiff <= upperDiff ? lower : upper;
	float adjustment = level / greyscale;

	if(racist_filter){
		gl_FragColor = vec4(color.r * adjustment, color.r * adjustment, color.r * adjustment, 1.0);
	}else{
		gl_FragColor = vec4(color.rgb * adjustment, 1.0);
	}
}