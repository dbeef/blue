layout (std140) uniform Matrices
{
// MVP
    mat4 view;
    mat4 projection;
// Light
    float ambientStrength;
    vec3 lightColor;
    vec3 lightPos;
// Shadows
    mat4 lightSpaceMatrix;
};
// https://stackoverflow.com/questions/48371519/how-to-acquire-the-sampler2d-type-from-a-shader-storage-buffer-object-inside-a-s
layout(location = 9) uniform sampler2D shadowMap;


uniform sampler2DArray ourTexture1;

in lowp vec2 TexCoord;
flat in lowp float Layer;
in lowp vec4 FragPosLightSpace;
out vec4 color;

in vec3 Normal;
in vec3 FragPos;

float ShadowCalculation(vec4 fragPosLightSpace)
{
    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;

    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;

    if(projCoords.z > 1.0)
        return 0.0f;

    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(shadowMap, projCoords.xy).r;

    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;

    const float bias = 0.001;
    // check whether current frag pos is in shadow

    float shadow = 0.0;
    vec2 texelSize = 1.0f / vec2(textureSize(shadowMap, 0).xy);

    // texelSize.y = 1.0f / float(textureSizeTemp.y);

    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
            shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
        }
    }

    return (shadow / 18.0f);
}

void main()
{
    vec3 _color = texture(ourTexture1, vec3(TexCoord.x, TexCoord.y, Layer)).rgb;
    vec3 normal = normalize(Normal);
    // ambient
    vec3 ambient = 0.85 * _color;
    // diffuse
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * lightColor;
    // specular
    const vec3 specular = vec3(0,0,0);
    // calculate shadow
    float shadow = ShadowCalculation(FragPosLightSpace);       
    vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * _color;


    //http://madebyevan.com/shaders/grid/
    // Pick a coordinate to visualize in a grid
    vec2 coord = TexCoord;
    // Compute anti-aliased world-space grid lines
    vec2 grid = abs(fract(coord - 0.5) - 0.5) / fwidth(coord);
    float line = min(grid.x, grid.y);
    vec4 grid_color = vec4(vec3(1.0 - min(line, 1.0)), 0.5);

    color = grid_color + vec4(lighting, 1.0);
}
