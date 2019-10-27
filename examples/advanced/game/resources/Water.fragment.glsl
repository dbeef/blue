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
out vec4 color;
in vec3 FragPos;

void main()
{
    vec2 point = FragPos.xz;
    vec2 center = cameraPos.xz;                                                                                                                                                            
    float radius = 25.0f;
    float distance = sqrt(pow(point.x - center.x, 2.0) + pow(point.y - center.y, 2.0));

    if (distance < radius)                                                                                                                                                                
color = vec4(ColorRGB, 1.0f);
    else                                                                                                                                                                           
color = vec4(ColorRGB + (0.1 * vec3(distance - radius)), 1.0f);
}
