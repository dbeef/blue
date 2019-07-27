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

// flat so it would not interpolate color
flat in lowp vec3 ColorRGB;
in vec3 vertex;

out vec4 color;

void main()
{
  // http://madebyevan.com/shaders/grid/
  // Pick a coordinate to visualize in a grid
  vec2 coord = vertex.xz;

  // Compute anti-aliased world-space grid lines
  vec2 grid = abs(fract(coord - 0.5) - 0.5) / fwidth(coord);
  float line = min(grid.x, grid.y);

  // Just visualize the grid lines directly
  vec3 grid_final = vec3(1.0 - min(line, 1.0));
  color = vec4(ColorRGB + grid_final, 1.0f);
}
