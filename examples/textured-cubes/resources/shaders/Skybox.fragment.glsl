uniform samplerCube skybox;

in vec3 TexCoords;
out vec4 color;

void main()
{
    color = texture(skybox, TexCoords);
}