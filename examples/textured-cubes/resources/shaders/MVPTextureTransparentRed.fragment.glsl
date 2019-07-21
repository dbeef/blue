uniform sampler2DArray ourTexture1;

in vec2 TexCoord;
flat in float Layer;

out vec4 color;

void main()
{
	color = vec4(1.0, 0.1, 0.1, 0.5);
}