uniform sampler2DArray ourTexture1;

in lowp vec2 TexCoord;
flat in lowp float Layer;

out vec4 color;

void main()
{
	color = texture(ourTexture1, vec3(TexCoord.x, TexCoord.y, Layer));
}