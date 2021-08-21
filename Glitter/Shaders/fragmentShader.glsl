#version 450 core
/*
 * 2 FragmentShader
 */
out vec4 FragColor;

uniform mat4 viewMatrix;
uniform mat4 projMatrix;
uniform vec3 cameraPos;
uniform vec3 lightPos;

in vec4 passPosition;
in vec4 passNormal;

float near = 0.1;
float far  = 100.0;

float LinearizeDepth(float depth)
{
    float z = depth * 2.0 - 1.0; // back to NDC
    return (2.0 * near * far) / (far + near - z * (far - near));
}

void main()
{
    vec3 viewDir = normalize(cameraPos - gl_FragCoord.xyz);

    vec4 ambient_color = vec4(0.2, 0.4, 0.2, 1);
    vec3 diffuse_color = vec3(0.4, 0.4, 0.2);
    vec3 specular_color = vec3(0.5, 0.5, 0);
    vec3 lightVector = normalize(lightPos - gl_FragCoord.xyz);

    //Diffuse
    float cos_phi = max(dot(passNormal.xyz, lightVector), 0.0f);

    //Specular

    vec3 reflection = normalize(reflect(-lightVector, passNormal.xyz));
    float cos_psi_n = pow(max(dot(viewDir, reflection), 0.0f), 8);


    FragColor = ambient_color;
    FragColor.rgb += diffuse_color * cos_phi;
    FragColor.rgb += specular_color * cos_psi_n;

    FragColor.a = 1;

//    float depth = LinearizeDepth(gl_FragCoord.z) / far; // divide by far for demonstration
//    FragColor = vec4(vec3(gl_FragCoord.z), 1.0); //Würde Depthtest funzen, gäbe es mehr als nur weiß.
}