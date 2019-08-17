layout(location = 9) uniform sampler2D sampler; // shadowmap

in lowp vec4 FragPosLightSpace;

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

// flat so it would not interpolate color
flat in lowp vec3 ColorRGB;

in vec3 Normal;
in vec3 FragPos;
out vec4 color;

float ShadowCalculation(vec4 fragPosLightSpace)
{
    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;

    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;

    if(projCoords.z > 1.0)
    return 0.0f;

    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(sampler, projCoords.xy).r;

    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;

    const float bias = 0.001;
    // check whether current frag pos is in shadow

    float shadow = 0.0;
    vec2 texelSize = 1.0f / vec2(textureSize(sampler, 0).xy);

    // texelSize.y = 1.0f / float(textureSizeTemp.y);

    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(sampler, projCoords.xy + vec2(x, y) * texelSize).r;
            shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
        }
    }

    return (shadow / 9.0f);
}

void main()
{
    vec3 lightColor = vec3(1.0f, 1.0f, 1.0f) * ColorRGB;

    vec3 normal = normalize(Normal);
    // ambient
    vec3 ambient = 0.75 * lightColor;
    // diffuse
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * lightColor;
    // specular
    const vec3 specular = vec3(0,0,0);
    // calculate shadow
    float shadow = ShadowCalculation(FragPosLightSpace);
    vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * lightColor;

    color = vec4(lighting, 1.0f);
}
