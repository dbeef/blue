layout(location = 0) in lowp vec3 position;
layout(location = 1) in lowp vec2 texCoord;
layout(location = 2) in lowp float layer;

// Values that stay constant for the whole mesh.
uniform vec3 CameraRight_worldspace;
uniform vec3 CameraUp_worldspace;
uniform mat4 VP; // Model-View-Projection matrix, but without the Model (the position is in BillboardPos; the orientation depends on the camera)

uniform vec3 BillboardPos;
uniform vec2 BillboardSize;

out lowp vec2 TexCoord;
flat out lowp float Layer;

// POSITION? FIXME
// Input vertex data, different for all executions of this shader.
// layout(location = 0) in vec3 squareVertices;

void main()
{
        vec3 particleCenter_wordspace = BillboardPos;

        vec3 vertexPosition_worldspace =
                particleCenter_wordspace
                + CameraRight_worldspace * position.x * BillboardSize.x
                + CameraUp_worldspace * position.y * BillboardSize.y;

        // Output position of the vertex
        gl_Position = VP * vec4(vertexPosition_worldspace, 1.0f);

        Layer = layer;
        TexCoord = vec2(texCoord.x, 1.0 - texCoord.y);
}
