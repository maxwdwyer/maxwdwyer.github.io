#version 100

precision mediump float;

varying vec3 fragNormal;
varying vec3 fragPosition;
varying vec2 fragTexCoord;
varying vec4 fragColor;

uniform vec3 lightDir;
uniform vec3 playerLoc;
uniform vec2 windowSize;

uniform sampler2D background;

float random(float x) {
    return fract(sin(x) * 43758.5453);
}

vec2 randomVec(vec2 pos) {
    float seed = pos.x * 12.9898 + pos.y * 78.233;
    float angle = random(seed) * 2.0 * 3.14159;

    return vec2(cos(angle), sin(angle));
}

vec3 randomVec3D(vec3 pos) {
    float seed = pos.x * 12.9898 + pos.y * 78.233 + pos.z * 22.123;
    float yaw = random(seed) * 2.0 * 3.14159;
    float pitch = random(seed + 1.0) * 2.0 * 3.14159;

    return vec3(cos(pitch) * sin(yaw), sin(pitch), cos(yaw) * cos(pitch));
}

float fade(float num) {
    return num * num * num * (num * (num * 6.0 - 15.0) + 10.0);
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

float perlin3D(float gridSize, vec3 position) {
    vec3 gridPos = vec3( floor(position.x / gridSize), floor(position.y / gridSize), floor(position.z / gridSize) );
    vec3 flooredPos = vec3( gridPos.x * gridSize, gridPos.y * gridSize, gridPos.z * gridSize );

    vec3 cornerOne = randomVec3D(flooredPos);
    vec3 cornerTwo = randomVec3D(vec3( flooredPos.x + gridSize, flooredPos.y, flooredPos.z ));
    vec3 cornerThree = randomVec3D(vec3( flooredPos.x, flooredPos.y + gridSize, flooredPos.z ));
    vec3 cornerFour = randomVec3D(vec3( flooredPos.x + gridSize, flooredPos.y + gridSize, flooredPos.z ));
    vec3 cornerFive = randomVec3D(vec3( flooredPos.x, flooredPos.y, flooredPos.z + gridSize));
    vec3 cornerSix = randomVec3D(vec3( flooredPos.x + gridSize, flooredPos.y, flooredPos.z + gridSize ));
    vec3 cornerSeven = randomVec3D(vec3( flooredPos.x, flooredPos.y + gridSize, flooredPos.z + gridSize ));
    vec3 cornerEight = randomVec3D(vec3( flooredPos.x + gridSize, flooredPos.y + gridSize, flooredPos.z + gridSize ));

    vec3 relPos = vec3( (position.x - flooredPos.x) / gridSize, (position.y - flooredPos.y) / gridSize, (position.z - flooredPos.z) / gridSize );

    vec3 disOne = vec3( relPos.x, relPos.y, relPos.z );
    vec3 disTwo = vec3( relPos.x - 1.0, relPos.y, relPos.z );
    vec3 disThree = vec3( relPos.x, relPos.y - 1.0, relPos.z );
    vec3 disFour = vec3( relPos.x - 1.0, relPos.y - 1.0, relPos.z );
    vec3 disFive = vec3( relPos.x, relPos.y, relPos.z - 1.0 );
    vec3 disSix = vec3( relPos.x - 1.0, relPos.y, relPos.z - 1.0 );
    vec3 disSeven = vec3( relPos.x, relPos.y - 1.0, relPos.z - 1.0 );
    vec3 disEight = vec3( relPos.x - 1.0, relPos.y - 1.0, relPos.z - 1.0 );

    float dotOne = dot(cornerOne, disOne);
    float dotTwo = dot(cornerTwo, disTwo);
    float dotThree = dot(cornerThree, disThree);
    float dotFour = dot(cornerFour, disFour);
    float dotFive = dot(cornerFive, disFive);
    float dotSix = dot(cornerSix, disSix);
    float dotSeven = dot(cornerSeven, disSeven);
    float dotEight = dot(cornerEight, disEight);

    float u = fade(relPos.x);
    float v = fade(relPos.y);
    float w = fade(relPos.z);

    float lerpOne = mix(dotOne, dotTwo, u);
    float lerpTwo = mix(dotThree, dotFour, u);
    float lerpThree = mix(dotFive, dotSix, u);
    float lerpFour = mix(dotSeven, dotEight, u);
    
    float lerpFive = mix(lerpOne, lerpTwo, v);
    float lerpSix = mix(lerpThree, lerpFour, v);

    return mix(lerpFive, lerpSix, w);
}

vec3 bump3D(float mp_gridSize, vec3 mp_position, float mp_offset, float scale) {
    vec3 vL = vec3(mp_position.x - mp_offset, mp_position.y, mp_position.z);
    vec3 vR = vec3(mp_position.x + mp_offset, mp_position.y, mp_position.z);
    vec3 vU = vec3(mp_position.x, mp_position.y - mp_offset, mp_position.z);
    vec3 vD = vec3(mp_position.x, mp_position.y + mp_offset, mp_position.z);
    vec3 vF = vec3(mp_position.x, mp_position.y, mp_position.z - mp_offset);
    vec3 vB = vec3(mp_position.x, mp_position.y, mp_position.z + mp_offset);

    float bL = perlin3D(mp_gridSize, vL);
    float bR = perlin3D(mp_gridSize, vR);
    float bU = perlin3D(mp_gridSize, vU);
    float bD = perlin3D(mp_gridSize, vD);
    float bF = perlin3D(mp_gridSize, vF);
    float bB = perlin3D(mp_gridSize, vB);

    float _x = (bL - bR); 
    float _y = (bU - bD);
    float _z = (bF - bB);

    vec3 vec = vec3(_x * scale, _y * scale, _z * scale);
    return vec;
}

void main()
{
    float distMult = 25.0;
    float dist = (0.2 - (distance(fragPosition, playerLoc) / distMult)) / 4.0;
    
    vec3 bump = (bump3D(0.5, fragPosition, 0.025, 0.125) * 2.0 + bump3D(0.25, fragPosition, 0.025, 0.125)) / 3.0;

    vec3 normal = normalize(fragNormal + (bump * 8.0));
    float diffuse = max(dot(normal, -lightDir), 0.0);
    float ambient = 0.4;

    float lighting = ambient + diffuse * 0.8;
    float num = max(lighting - dist, 0.5);
    

    float fogStart = 10.0;
    float fogEnd = 64.0;

    float distanceFromPlayer = distance(fragPosition, playerLoc);
    float fogFactor = smoothstep(fogStart, fogEnd, distanceFromPlayer);
    vec4 white = vec4(1.0, 1.0, 1.0, 0.7);

    float frost = (perlin3D(0.25, fragPosition) * 2.0 + perlin3D(0.125, fragPosition)) / 3.0;

    vec2 wCoords = vec2(gl_FragCoord.x / windowSize.x + bump.x, gl_FragCoord.y / windowSize.y + bump.z);
    vec4 backgroundColor = texture2D(background, wCoords);
    vec4 back = vec4(backgroundColor.r * lighting + 0.02, backgroundColor.g * lighting + 0.05, backgroundColor.b * lighting + 0.08, 1.0);

    float edgeFrost = (length(fragTexCoord - vec2(0.5, 0.5)) - (frost * 0.5) - 0.35);
    edgeFrost = clamp(edgeFrost, 0.0, 1.0) * 1.5 + 0.1;

    gl_FragColor = mix(mix(back, white, fogFactor), white, edgeFrost);
}