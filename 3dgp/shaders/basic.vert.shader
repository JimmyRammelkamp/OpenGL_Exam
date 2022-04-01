#version 330

// Matrices
uniform mat4 matrixProjection;
uniform mat4 matrixView;
uniform mat4 matrixModelView;

// Materials
uniform vec3 materialAmbient;
uniform vec3 materialDiffuse;

// Lights
struct AMBIENT
{
	vec3 color;
};
struct DIRECTIONAL
{	
	vec3 direction;
	vec3 diffuse;
};

uniform AMBIENT lightAmbient, lightEmissive;
uniform DIRECTIONAL lightDir;

in vec3 aVertex;
in vec3 aNormal;

// Output (sent to Fragment Shader)
out vec4 color;
out vec4 position;
out vec3 normal;

vec4 AmbientLight(AMBIENT light)
{
	return vec4(materialAmbient * light.color, 1);
}

vec4 DirectionalLight(DIRECTIONAL light)
{
	vec3 L = normalize(mat3(matrixView) * light.direction).xyz;
	float NdotL = dot(normal, L);
	if (NdotL > 0)
		return vec4(light.diffuse * materialDiffuse * NdotL, 1);
	else
		return vec4(0, 0, 0, 1);
}

void main(void) 
{
	// calculate position & normal
	position = matrixModelView * vec4(aVertex, 1.0);
	gl_Position = matrixProjection * position;
	normal = normalize(mat3(matrixModelView) * aNormal);

	// calculate the colour
	color = vec4(0, 0, 0, 0);

	// lights
	color += AmbientLight(lightAmbient);
	color += AmbientLight(lightEmissive);
	color += DirectionalLight(lightDir);
}
