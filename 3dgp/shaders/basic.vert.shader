#version 330

// Matrices
uniform mat4 matrixProjection;
uniform mat4 matrixView;
uniform mat4 matrixModelView;

// Materials
uniform vec3 materialAmbient;
uniform vec3 materialDiffuse;

// Bone Transforms
#define MAX_BONES 100
uniform mat4 bones[MAX_BONES];

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
in vec2 aTexCoord;
in vec3 aTangent;
in vec3 aBiTangent;
in ivec4 aBoneId;		// Bone Ids
in  vec4 aBoneWeight;	// Bone Weights


// Output (sent to Fragment Shader)
out vec4 color;
out vec4 position;
out vec3 normal;
out vec2 texCoord0;
out mat3 matrixTangent;

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
	mat4 matrixBone;
	if (aBoneWeight[0] == 0.0)
		matrixBone = mat4(1);
	else
		matrixBone = (bones[aBoneId[0]] * aBoneWeight[0] +
			bones[aBoneId[1]] * aBoneWeight[1] +
			bones[aBoneId[2]] * aBoneWeight[2] +
			bones[aBoneId[3]] * aBoneWeight[3]);

	// calculate position & normal
	position = matrixModelView * matrixBone * vec4(aVertex, 1.0);
	gl_Position = matrixProjection * position;
	normal = normalize(mat3(matrixModelView) *mat3(matrixBone) * aNormal);

	// calculate tangent local system transformation
	vec3 tangent = normalize(mat3(matrixModelView) * aTangent);
	vec3 biTangent = normalize(mat3(matrixModelView) * aBiTangent);
	matrixTangent = mat3(tangent, biTangent, normal);


	texCoord0 = aTexCoord;

	// calculate the colour
	color = vec4(0, 0, 0, 0);

	// lights
	color += AmbientLight(lightAmbient);
	color += AmbientLight(lightEmissive);
	color += DirectionalLight(lightDir);
}
