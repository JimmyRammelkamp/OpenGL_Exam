#version 330

in vec4 color;
in vec4 position;
in vec3 normal;
in vec2 texCoord0;
in mat3 matrixTangent;

out vec4 outColor;

uniform vec3 materialDiffuse;
uniform mat4 matrixView;

uniform float time;
uniform float speedX;
uniform float speedY;

uniform sampler2D texture0;
uniform sampler2D textureNormal;

uniform bool useNormalMap = false;

// Debug Text - Do Not Change!
uniform int Text = 0;

vec3 normalNew;

struct POINT
{	
	vec3 position;
	vec3 diffuse;
};
uniform POINT lightPoint;

vec4 PointLight(POINT light)
{
	vec4 color = vec4(0, 0, 0, 0);
	vec3 L = (normalize((matrixView * vec4(light.position, 1)) - position)).xyz;
	float NdotL = dot(normalNew, L);
	if (NdotL > 0)
		color += vec4(materialDiffuse * light.diffuse, 1) * NdotL;

	/*vec3 V = normalize(-position.xyz);
	vec3 R = reflect(-L, normalNew);
	float RdotV = dot(R, V);
	if (NdotL > 0 && RdotV > 0)
		color += vec4(materialSpecular * light.specular * pow(RdotV, shininess), 1);*/

	float dist = length(matrixView * vec4(light.position, 1) - position);
	float att = 1 / (0.03 * dist * dist); //att_quadratic hard coded to 0.03 would be good to make uniform float variable


	return color * att;

}

void main(void) 
{
	if (useNormalMap)
	{
		normalNew = 2.0 * texture(textureNormal, texCoord0).xyz - vec3(1.0, 1.0, 1.0);
		normalNew = normalize(matrixTangent * normalNew);
	}
	else
		normalNew = normal;

	float xScrollValue = speedX * time;
	float yScrollValue = speedY * time;

	outColor = color;
	outColor += PointLight(lightPoint);
	outColor *= texture(texture0, texCoord0 + vec2(xScrollValue, yScrollValue));

	// Debug Text - Do Not Change!
	if (Text == 1) outColor = vec4(1, 0.8, 0, 1);
}
