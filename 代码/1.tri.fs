#version 330 core
out vec4 FragColor;

struct Material {
    // vec3 ambient;
    // vec3 diffuse;
    // sampler2D diffuse;
    // sampler2D specular;    
    float shininess;
}; 
struct DirLight {
    vec3 direction;
	
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight {
    vec3 position;
    
    float constant;
    float linear;
    float quadratic;
	
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

#define NR_POINT_LIGHTS 4

in vec3 Normal;  
in vec3 FragPos; 
in float Cut;
  
uniform DirLight dirLight;
uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform vec3 viewPos;
uniform Material material;

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

void main()
{
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);

    vec3 result = CalcDirLight(dirLight, norm, viewDir);
    for(int i = 0; i < NR_POINT_LIGHTS; i++)
        result += CalcPointLight(pointLights[i], norm, FragPos, viewDir);     

    FragColor = vec4(result, 1.0);
    // FragColor = vec4(1.0,1.0,1.0, 1.0);
    	
}

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);
    vec3 a=vec3(0.4,0.4,0.4);
    vec3 d=vec3(0.5,0.5,0.5);
    vec3 s=vec3(0.6,0.6,0.6);
    if(Cut==1.0)//切割了，更亮
    {
        a=vec3(0.7,0.7,0.7);
        d=vec3(0.9,0.9,0.9);
        s=vec3(1.0,1.0,1.0);
        //lightDir=lightDir*vec3(-1.0,-1.0,-1.0);
    }
    else{
        // a=vec3(0.5,0.5,0.5);
        // d=vec3(0.7,0.7,0.7);
        // s=vec3(0.8,0.8,0.8);
    }

    // vec3 lightDir = normalize(-light.direction);
    
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // combine results
    vec3 ambient = light.ambient  * a;
    vec3 diffuse = light.diffuse * (diff *d);
    vec3 specular = light.specular * (spec *s);
    return (ambient + diffuse + specular);
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 a=vec3(0.4,0.4,0.4);
    vec3 d=vec3(0.6,0.6,0.6);
    vec3 s=vec3(0.4,0.4,0.4);
    if(Cut==1.0)//切割了，更亮
    {
        a=vec3(0.7,0.7,0.7);
        d=vec3(0.9,0.9,0.9);
        s=vec3(0.9,0.9,0.9);
    }
    else{
        // a=vec3(0.5,0.5,0.5);
        // d=vec3(0.7,0.7,0.7);
        // s=vec3(0.4,0.4,0.4);
    }
    vec3 lightDir = normalize(light.position - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    // combine results
    vec3 ambient = light.ambient *a;
    vec3 diffuse = light.diffuse * (diff *d);
    vec3 specular = light.specular * (spec *s);
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    return (ambient + diffuse + specular);
}