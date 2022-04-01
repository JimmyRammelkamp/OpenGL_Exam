#version 330

in vec4 color;
in vec4 position;
in vec3 normal;

out vec4 outColor;

uniform vec3 materialDiffuse;
uniform mat4 matrixView;

// Debug Text - Do Not Change!
uniform int Text = 0;

struct POINT
{	
	vec3 position;
	vec3 diffuse;
};
uniform POINT lightPoint;

void main(void) 
{
	outColor = color;


	// Debug Text - Do Not Change!
	if (Text == 1) outColor = vec4(1, 0.8, 0, 1);
}
