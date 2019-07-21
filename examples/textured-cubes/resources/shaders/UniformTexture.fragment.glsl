// Texture samplers
uniform sampler2D ourTexture1;
uniform sampler2D ourTexture2;
uniform vec4 colorToAdd;

in vec2 TexCoord;
out vec4 color;

void main()
{
	// Linearly interpolate between both textures (second texture is only slightly combined)
	color = mix(texture(ourTexture1, TexCoord), texture(ourTexture2, TexCoord), 0.2);
	color += colorToAdd;
}