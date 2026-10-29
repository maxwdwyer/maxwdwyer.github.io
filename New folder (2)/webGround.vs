#version 100

precision mediump float;

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

float random(float x) {
    return fract(sin(x) * 43758.5453);
}

vec3 randomVec3D(vec3 pos) {
    float seed = pos.x * 12.9898 + pos.y * 78.233 + pos.z * 22.123;
    float yaw = random(seed) * 2.0 * 3.14159;
    float pitch = random(seed + 1.0) * 2.0 * 3.14159;

    return vec3(cos(pitch) * sin(yaw), sin(pitch), cos(yaw) * cos(pitch));
}

void main()
{
    vec4 worldPosition = matModel * vec4(vertexPosition, 1.0);

    fragPosition = worldPosition.xyz;
    fragNormal = mat3(matModel) * vertexNormal;

    fragTexCoord = vertexTexCoord;
    fragColor = vertexColor;

    vec3 displaced = vertexPosition + (randomVec3D(vertexPosition) * 0.125);

    gl_Position = mvp * vec4(displaced, 1.0);
}