#version 330

in vec3 fragNormal;
in vec3 fragPosition;
in vec2 fragTexCoord;

uniform sampler2D texture0;
uniform vec3 lightDir;
uniform vec3 playerLoc;

out vec4 finalColor;

void main()
{
    float distMult = 25.0;
    float dist = (.2 - (distance(fragPosition, playerLoc) / distMult)) / 1.5;
 
    vec3 normal = normalize(fragNormal);
    float diffuse = max(dot(normal, -lightDir), 0.0);
    float ambient = .4;

    float lighting = ambient + diffuse * 0.8;
    float num = max(lighting - dist, .5);

    vec4 texColor = texture(texture0, fragTexCoord);

    float fogStart = 10.;
    float fogEnd = 64.;

    float distanceFromPlayer = distance(fragPosition, playerLoc);
    float fogFactor = smoothstep(fogStart, fogEnd, distanceFromPlayer);
    //vec4 white = vec4(.93, .96, 1., 1.);
    vec4 white = vec4(1., 1., 1., 1.);

    float red = num * texColor.r * (125. / 256.);
    float green = num * texColor.g * (94. / 256.);
    float blue = num * texColor.b * (56. / 256.);

    vec4 sceneColor = vec4(red, green, blue, texColor.a);

    finalColor = mix(sceneColor, white, fogFactor);
}