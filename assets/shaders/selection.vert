#version 330 core

layout (location = 0) in vec3 vertex;
layout (location = 1) in vec3 color;

uniform float pulse;
uniform mat4 viewproj;
uniform vec3 clip_plane;

flat out vec4 inclr;

void
main()
{
	vec4 pos = vec4(vertex, 1.0);

	float br = (cos(pulse * 15) + 1) * 0.5;

	inclr = vec4(color * br, 0.8);

	gl_Position = viewproj * pos;

	float above_water = dot(pos, vec4(0, 1, 0, 0));
	gl_ClipDistance[0] = dot(vec3(0, above_water, -above_water), clip_plane);
}
