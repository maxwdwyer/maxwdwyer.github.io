#version 330

in vec3 fragNormal;
in vec3 fragPosition;
in vec2 fragTexCoord;

uniform sampler2D texture0;
uniform vec3 lightDir;
uniform vec3 playerLoc;
uniform vec3 camDir;

out vec4 finalColor;

// Input lighting values
uniform vec3 sunLightDir;
uniform vec4 sunLightColor;
uniform vec4 ambient;
uniform vec3 viewPos;

// Input shadowmapping values
uniform mat4 lightVP; // Light source view-projection matrix
uniform sampler2D shadowMap;

uniform int shadowMapResolution;


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
    float pitch = random(seed + 1) * 2.0 * 3.14159;

    return vec3(cos(pitch) * sin(yaw), sin(pitch), cos(yaw) * cos(pitch));
}

float fade(float num) {
    return num * num * num * (num * (num * 6 - 15) + 10);
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

float multiPerlin3D(float gridSize, vec3 position, int l, float scale, float influence) {
    float V = 0.;
    float num = 1.;
    float scaledNum = 1.;

    float change = (2. - influence);

    for (int i = 0; i < l; i++){
        V *= change;
        num *= change;
        scaledNum *= (change * scale);
        V += perlin3D(gridSize / scaledNum, position);
    };

    return V / num;
}

vec3 bump3D(float mp_gridSize, vec3 mp_position, float mp_offset, int mp_layers, float mp_scale, float mp_influence, float scale) {
    vec3 vL = vec3(mp_position.x - mp_offset, mp_position.y, mp_position.z);
    vec3 vR = vec3(mp_position.x + mp_offset, mp_position.y, mp_position.z);
    vec3 vU = vec3(mp_position.x, mp_position.y - mp_offset, mp_position.z);
    vec3 vD = vec3(mp_position.x, mp_position.y + mp_offset, mp_position.z);
    vec3 vF = vec3(mp_position.x, mp_position.y, mp_position.z - mp_offset);
    vec3 vB = vec3(mp_position.x, mp_position.y, mp_position.z + mp_offset);

    float bL = multiPerlin3D(mp_gridSize, vL, mp_layers, mp_scale, mp_influence);
    float bR = multiPerlin3D(mp_gridSize, vR, mp_layers, mp_scale, mp_influence);
    float bU = multiPerlin3D(mp_gridSize, vU, mp_layers, mp_scale, mp_influence);
    float bD = multiPerlin3D(mp_gridSize, vD, mp_layers, mp_scale, mp_influence);
    float bF = multiPerlin3D(mp_gridSize, vF, mp_layers, mp_scale, mp_influence);
    float bB = multiPerlin3D(mp_gridSize, vB, mp_layers, mp_scale, mp_influence);

    float _x = (bL - bR); 
    float _y = (bU - bD);
    float _z = (bF - bB);

    vec3 vec = vec3(_x * scale, _y * scale, _z * scale);
    return vec;
}

void main() {
    float distMult = 25.0;
    float dist = (.2 - (distance(fragPosition, playerLoc) / distMult)) / 4;

    float mp_gridSize = 2;
    vec3 mp_position = vec3(fragPosition.x, fragPosition.z, fragPosition.y);
    float mp_offset = .05;
    int mp_layers = 2;
    float mp_scale = 1.5;
    float mp_influence = .1;
    float scale = 1.6;

    vec3 bumpNormal = bump3D(mp_gridSize, mp_position, mp_offset, mp_layers, mp_scale, mp_influence, scale);

    vec3 normal = normalize(fragNormal + bumpNormal);
    float diffuse = max(dot(normal, -lightDir), 0.0);

    float ambient = .4;
    float brightness = .9;

    float lighting = ambient + diffuse * brightness;
    float shadeInfluence = max(lighting - dist, .1);

    float red = lighting;
    float green = lighting;
    float blue = lighting;


    // ***SPARKLES***
    float sparkleFactor = .35;
    float sparkleMult = .6;
    float dullSparkleFactor = 1.6;

    float glance = 1. - dot(normal, camDir) - sparkleFactor;
    if (glance > 0.) {
        if (random(glance) * glance > sparkleMult) {
            red += 1.;
            green += 1.;
            blue += 1.;
        }
    }

    vec3 rand = randomVec3D(fragPosition);
    float A = dullSparkleFactor / 3.;
    if (rand.r > A && rand.g > A && rand.b > A) {
        red += sparkleMult;
        green += sparkleMult;
        blue += sparkleMult;
    }

    //--------------------------------------------------------

    float fogStart = 10.;
    float fogEnd = 64.;

    float distanceFromPlayer = distance(fragPosition, playerLoc);
    float fogFactor = smoothstep(fogStart, fogEnd, distanceFromPlayer);
    vec4 white = vec4(1., 1., 1., .85);

    vec4 sceneColor = vec4(red * 1.3, green * 1.3, blue * 1.5, .4);

    //finalColor = mix(sceneColor, vec4(0., 0., 0., 1.), shadowInfluence);
    finalColor = mix(sceneColor, white, fogFactor);
}