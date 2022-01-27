#version 330 core
out vec4 FragColor;

struct Material {
    // vec3 ambient;
    // vec3 diffuse;
    sampler2D diffuse;
    sampler2D specular;    
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
in vec2 TexCoords; 
//in float Cut;
  
// uniform vec3 lightPos;   
// uniform vec3 objectColor;
// uniform vec3 lightColor;
uniform DirLight dirLight;
uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform vec3 viewPos;
uniform Material material;

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
// vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

void main()
{
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);

    // vec3 lightDir = normalize(light.position - FragPos);//点的位置向量的差
    // // vec3 lightDir = normalize(-light.direction);
    // float diff = max(dot(norm, lightDir), 0.0);//向量点乘即可。
    // vec3 diffuse =  light.diffuse * diff * texture(material.diffuse, TexCoords).rgb;  

    // // specular
    // vec3 viewDir = normalize(viewPos - FragPos);
    // vec3 reflectDir = reflect(-lightDir, norm);  
    // float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // vec3 specular = light.specular * spec * texture(material.specular, TexCoords).rgb;  

    // float distance    = length(light.position - FragPos);
    // float attenuation = 1.0 / (light.constant + light.linear * distance + 
    //             light.quadratic * (distance * distance));

    // ambient  *= attenuation;  
    // diffuse   *= attenuation;
    // specular *= attenuation;   

    vec3 result = CalcDirLight(dirLight, norm, viewDir);
    for(int i = 0; i < NR_POINT_LIGHTS; i++)
        result += CalcPointLight(pointLights[i], norm, FragPos, viewDir);     

    // if(Cut==1.0)
    // {
    //     FragColor = vec4(result, 1.0);
    // }
    // else{
    //     FragColor = vec4(1.0,1.0,1.0, 1.0);
    // }//是可以判断的！！

    FragColor = vec4(result, 1.0);
    	
}

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
    // vec3 a=vec3(1.0,1.0,1.0);
    // vec3 d=vec3(1.0,1.0,1.0);
    // vec3 s=vec3(1.0,1.0,1.0);
    vec3 a=vec3(1.5,1.5,1.5);
    vec3 d=vec3(1.0,1.0,1.0);
    vec3 s=vec3(0.8,0.8,0.8);
    // if(Cut==1.0)//切割了，更亮
    // {
    //     a=vec3(0.7,0.7,0.7);
    //     d=vec3(0.9,0.9,0.9);
    //     // a=vec3(1.0,1.0,1.0);
    //     // d=vec3(1.0,1.0,1.0);
    //     s=vec3(1.0,1.0,1.0);
    // }
    // else{
    //     a=vec3(0.5,0.5,0.5);
    //     d=vec3(0.7,0.7,0.7);
    //     s=vec3(0.8,0.8,0.8);
    // }

    vec3 lightDir = normalize(-light.direction);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // combine results
    vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords)) * a;
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoords))*d;
    vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords))*s;
    return (ambient + diffuse + specular);
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 a=vec3(1.5,1.5,1.5);
    vec3 d=vec3(1.0,1.0,1.0);
    vec3 s=vec3(0.8,0.8,0.8);
    // if(Cut==1.0)//切割了，更亮
    // {
    //     a=vec3(0.7,0.7,0.7);
    //     d=vec3(0.9,0.9,0.9);
    //     // a=vec3(1.0,1.0,1.0);
    //     // d=vec3(1.0,1.0,1.0);
    //     s=vec3(1.0,1.0,1.0);
    // }
    // else{
    //     a=vec3(0.5,0.5,0.5);
    //     d=vec3(0.7,0.7,0.7);
    //     s=vec3(0.4,0.4,0.4);
    // }
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
    vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords))*a;
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoords))*d;
    vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords))*s;
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    return (ambient + diffuse + specular);
}