#version 450 core
/*
 * 2 FragmentShader
 */
out vec4 FragColor;

uniform mat4 viewMatrix;
uniform mat4 projMatrix;
uniform vec3 cameraPos;
uniform vec3 lightPos;
uniform sampler2D sand, grass1, grass, rock, snow, rockNormal;

in vec4 passPosition;
in vec4 passNormal;
in vec2 passTex;
in float visibility;

vec3 fogColor = vec3(0.5, 0.6, 0.7);

float fogFalloff = 0.000075f;
float gDispFactor = 20.0;
const float c = 18.;
const float b = 3.e-6;

float applyFog(in vec3  rgb, // original color of the pixel
in float dist, // camera to point distance
in vec3  cameraPos, // camera position
in vec3  rayDir)// camera to point vector
{
    float fogAmount = c * exp(-cameraPos.y*fogFalloff) * (1.0-exp(-dist*rayDir.y*fogFalloff))/rayDir.y;
    vec3  fogColor  = vec3(0.5, 0.6, 0.7);
    return clamp(fogAmount, 0.0, 1.0);//mix( rgb, fogColor, fogAmount );

}
void main()
{
    vec2 u_FogDist = vec2(2500.0, 10000.0);
    vec3 worldPos = passPosition.xyz;
    float fogFactor = applyFog(vec3(0.0), distance(cameraPos, worldPos), cameraPos, normalize(worldPos - cameraPos));

    vec3 viewDir = normalize(cameraPos - passPosition.xyz);

    vec4 ambient_color = vec4(0.2, 0.4, 0.2, 1);
    vec3 diffuse_color = vec3(0.4, 0.4, 0.2);
    //    vec3 specular_color = vec3(0.5, 0.5, 0);
    vec3 lightVector = normalize(lightPos - passPosition.xyz);

    //Diffuse
    float cos_phi = max(dot(passNormal.xyz, lightVector), 0.0f);

    //Specular

    vec3 reflection = normalize(reflect(-lightVector, passNormal.xyz));
    float cos_psi_n = pow(max(dot(viewDir, reflection), 0.0f), 8);

    FragColor = vec4(texture(rock, passTex).rgb, 1.0);
    //    FragColor += ambient_color;
    FragColor.rgb += diffuse_color * cos_phi;
    FragColor.rgb += FragColor.rgb * cos_psi_n;
    vec3 color = FragColor.rgb;
    FragColor = mix(FragColor, vec4(mix(fogColor*1.1, fogColor*0.85, clamp(worldPos.y/(1500.*16.)*gDispFactor, 0.0, 1.0)), 1.0f), fogFactor);
    FragColor.a = 1;
    //    FragColor = mix(vec4(0.9, 0.9, 0.95, 1.0),  vec4(FragColor.rgb, 1.0), visibility);
}