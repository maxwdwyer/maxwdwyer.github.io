
#version 100

attribute vec3 vertexPosition;
attribute vec2 vertexTexCoord;
attribute vec3 vertexNormal;
attribute vec4 vertexColor;

uniform mat4 mvp;
uniform mat4 matModel;

varying vec3 fragNormal;
varying vec3 fragPosition;
varying vec2 fragTexCoord;
varying vec4 fragColor;

void main()
{
    vec4 worldPosition = matModel * vec4(vertexPosition, 1.0);

    fragPosition = worldPosition.xyz;
    fragNormal = mat3(matModel) * vertexNormal;

    fragTexCoord = vertexTexCoord;
    fragColor = vertexColor;

    gl_Position = mvp * vec4(vertexPosition, 1.0);
}
