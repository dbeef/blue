layout (std140) uniform Matrices
{
// MVP
    mat4 view;
    mat4 projection;
// Light
    float ambientStrength; 
    vec3 lightColor;
    vec3 lightPos;
    vec3 cameraPos;
// Shadows
    mat4 lightSpaceMatrix;
};

in vec3 Normal;
in vec2 TextureCoordinates;
in vec3 FragPos;

in vec3 MaterialAmbient;
in vec3 MaterialDiffuse;
in vec3 MaterialSpecular;

in vec3 ToCameraPos; // For fresnel effect

in float MaterialShininess;

out vec4 color;

void main()
{
//    vec3 lightColor = vec3(1.0f, 1.0f, 1.0f);
//    vec3 ambient = ambientStrength * lightColor;

//    vec3 norm = normalize(Normal);
//    vec3 lightDir = normalize(lightPos - FragPos);

//    float diff = max(dot(norm, lightDir), 0.0);
//    vec3 diffuse = diff * lightColor;

//    color = vec4(ambient + diffuse, 1.0f);

    // ambient
    vec3 ambient = lightColor * ambientStrength * MaterialAmbient;
  	
    // diffuse 
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = lightColor * (diff * MaterialDiffuse);
    
    // specular

    vec3 viewDir = normalize(cameraPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), MaterialShininess);
    vec3 specular = lightColor * (spec * MaterialSpecular);  

	// fresnel

	//vec3 toCameraVector = cameraPos - FragPos;
	//vec3 viewVector = normalize(toCameraVector);
	//float refractiveFactor = clamp(pow(dot(viewDir, norm), 10), 0.0, 1.0);
	//vec3 refractive = vec3(0.5, 0.5, 0.5) * refractiveFactor;
        
    vec3 result = ambient + diffuse + specular;// + refractive;
    color = vec4(result, 1.0);
}
