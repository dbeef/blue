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

uniform sampler2DArray ourTexture1;


in lowp vec2 TexCoord;
flat in lowp float Layer;

out vec4 color;

in vec3 Normal;
in vec3 FragPos;


void main()
{
    vec3 ambient = ambientStrength * lightColor;

    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);

    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;


  //http://madebyevan.com/shaders/grid/

  // Pick a coordinate to visualize in a grid
  vec2 coord = TexCoord;

  // Compute anti-aliased world-space grid lines
  vec2 grid = abs(fract(coord - 0.5) - 0.5) / fwidth(coord);
  float line = min(grid.x, grid.y);

  // Just visualize the grid lines directly
  color = (vec4(ambient + diffuse, 1.0f) * texture(ourTexture1, vec3(TexCoord.x, TexCoord.y, Layer)));
}
