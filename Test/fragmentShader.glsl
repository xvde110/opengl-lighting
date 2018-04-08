#version 330 core

in vec3 Normal;
in vec2 TexCoord;
in vec3 worldPos;

out vec4 color;

layout(location = 0) out vec3 color1;
layout(location = 1) out vec3 color2;
layout(location = 2) out vec3 color3;
layout(location = 3) out vec3 color4;

uniform vec3 cameraPos;


struct Material{
	sampler2D diffuse;
	sampler2D specular;
	float shininess;
};

uniform Material material;

struct Light{
	//all need
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	//point light need
	vec3 position;
	float constant;
    float linear;
    float quadratic;
	//direction light need
	vec3 direction;
	//spot light
	float cutoff;
	float outercutoff;
};

uniform Light light;

void main()
{


	//ambient
	vec3 ambient=light.ambient * vec3(texture(material.diffuse,TexCoord));

	//diffuse
	vec3 norm=normalize(Normal);

	//point light get lightDir
	vec3 lightDir=normalize(light.position-worldPos);
	//π‚À•ºı
	float distance=length(light.position-worldPos);
	float attenuation=1.0f/(light.constant+light.linear*distance+light.quadratic*distance*distance);

	//direction light get lightDir
	//vec3 lightDir=normalize(-light.direction);

	float diff=max(dot(norm,lightDir),0.0);
	vec3 diffuse=light.diffuse * diff * vec3(texture(material.diffuse,TexCoord));

	//specular
	vec3 viewDir=normalize(cameraPos-worldPos);
	vec3 reflectDir=reflect(-lightDir,norm);
	float spec=pow(max(dot(viewDir,reflectDir),0.0),material.shininess);
	vec3 specular=light.specular * spec * vec3(texture(material.specular,TexCoord));

	color=vec4((ambient+diffuse+specular)*attenuation,1.0);

	//spot light
	float theta = dot(lightDir, normalize(-light.direction));
	float epsilon = light.cutoff - light.outercutoff;
	float intensity = clamp((theta - light.outercutoff) / epsilon,0.0, 1.0);

	color=vec4((ambient+(diffuse+specular)*intensity)*attenuation,1.0);

	color=vec4((ambient+(diffuse+specular)),1.0);

	color1=color.xyz;
	color2=vec3(1.0)-color1;
	color3=color1*2;
	color4=color1/2;
}