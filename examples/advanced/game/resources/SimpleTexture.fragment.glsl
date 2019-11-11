layout(location = 9) uniform sampler2D sampler;

in vec2 TexCoord;

out vec4 color;

void main()
{
    color = texture(sampler, TexCoord);
}
