#version 330 core

uniform float levels;
uniform bool bnw;
uniform vec2 u_resolution;
uniform sampler2D tex;
uniform float pixel_size;

uniform bool use_old_posterizer = false;

void main(){
	vec2 uv = gl_FragCoord.xy / u_resolution.xy;

	vec2 coord = floor(gl_FragCoord.xy / pixel_size) * pixel_size;
	vec2 new_uv = coord / u_resolution.xy;

/*-----------------OLD POSTERIZER---------------------------------*/
if(use_old_posterizer){
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
/*------------------NEW POSTERIZER-------------------------------------*/
}else{
	float region_size = float(1.0 / levels);
	vec3 pixel_value = texture(tex, new_uv).rgb;
	//vec3 pixel_value = vec3(original_value.r * 255.0, original_value.r * 255.0, original_value.r * 255.0);
	vec3 i_region = vec3(floor(pixel_value.r / region_size), floor(pixel_value.g / region_size), floor(pixel_value.b / region_size));
	float delta_r = 1.0 / (levels - 1.0);
	vec3 final_value = vec3(i_region.r * delta_r, i_region.g * delta_r, i_region.b * delta_r);
	
	if(bnw){
		final_value = vec3(i_region.r * delta_r);
	}

	gl_FragColor = vec4(final_value, 1.0);
	}
}