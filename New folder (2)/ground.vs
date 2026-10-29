#version 330

in vec3 vertexPosition;
in vec3 vertexNormal;
in vec2 vertexTexCoord;

uniform mat4 mvp;
uniform mat4 matModel;

out vec3 fragNormal;
out vec3 fragPosition;
out vec2 fragTexCoord;


float random(float x) {
    return fract(sin(x) * 43758.5453);
}

vec2 randomVec(vec2 pos) {
    float seed = pos.x * 12.9898 + pos.y * 78.233;
    float angle = random(seed) * 2.0f * 3.14159;

    return vec2(cos(angle), sin(angle));
}

float fade(float num) {
    return num * num * num * (num * (num * 6 - 15) + 10);
}

float perlin(float gridSize, vec2 position) {
    vec2 gridPos = vec2( floor(position.x / gridSize), floor(position.y / gridSize) );
    vec2 flooredPos = vec2( gridPos.x * gridSize, gridPos.y * gridSize );

    vec2 cornerOne = randomVec(flooredPos);
    vec2 cornerTwo = randomVec(vec2( flooredPos.x + gridSize, flooredPos.y ));
    vec2 cornerThree = randomVec(vec2( flooredPos.x, flooredPos.y + gridSize ));
    vec2 cornerFour = randomVec(vec2( flooredPos.x + gridSize, flooredPos.y + gridSize ));

    vec2 relPos = vec2( (position.x - flooredPos.x) / gridSize, (position.y - flooredPos.y) / gridSize );

    vec2 disOne = vec2( relPos.x, relPos.y );
    vec2 disTwo = vec2( relPos.x - 1.0, relPos.y );
    vec2 disThree = vec2( relPos.x, relPos.y - 1.0 );
    vec2 disFour = vec2( relPos.x - 1.0, relPos.y - 1.0 );

    float dotOne = dot(cornerOne, disOne);
    float dotTwo = dot(cornerTwo, disTwo);
    float dotThree = dot(cornerThree, disThree);
    float dotFour = dot(cornerFour, disFour);

    float u = fade(relPos.x);
    float v = fade(relPos.y);

    float lerpOne = mix(dotOne, dotTwo, u);
    float lerpTwo = mix(dotThree, dotFour, u);
    return mix(lerpOne, lerpTwo, v);
}


void main()
{
    fragPosition = vec3(matModel * vec4(vertexPosition, 1.0));
    fragNormal = mat3(transpose(inverse(matModel))) * vertexNormal;
    fragTexCoord = vertexTexCoord;

    float pixelSize = 8.;
    vec3 fragPosition2 = vec3(
        floor(fragPosition.x * pixelSize) / pixelSize,
        floor(fragPosition.y * pixelSize) / pixelSize,
        floor(fragPosition.z * pixelSize) / pixelSize
    );

    float bumpStrength = 0.55;

    float height = perlin(2.0, fragPosition2.xz);

    vec3 displaced = vertexPosition;
    displaced.y += height * bumpStrength;


    gl_Position = mvp * vec4(displaced, 1.0);
}