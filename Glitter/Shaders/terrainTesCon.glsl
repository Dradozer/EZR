#version 410 core

// define the number of CPs in the output patch
layout (vertices = 3) out;

uniform vec3 gEyeWorldPos;

// attributes of the input CPs
in vec4 passPosition_c[];
in vec4 passNormal_c[];

// attributes of the output CPs
out vec4 passPosition_e[];
out vec4 passNormal_e[];

float GetTessLevel(float Distance0, float Distance1)
{
    float AvgDistance = (Distance0 + Distance1) / 2.0;

    if (AvgDistance <= 2.0) {
        return 10.0;
    }
    else if (AvgDistance <= 5.0) {
        return 7.0;
    }
    else {
        return 3.0;
    }
}

void main()
{
    // Set the control points of the output patch
    passNormal_e[gl_InvocationID]   = passNormal_c[gl_InvocationID];
    passPosition_e[gl_InvocationID] = passPosition_c[gl_InvocationID];

    // Calculate the distance from the camera to the three control points
    float EyeToVertexDistance0 = distance(gEyeWorldPos, passPosition_e[0].xyz);
    float EyeToVertexDistance1 = distance(gEyeWorldPos, passPosition_e[1].xyz);
    float EyeToVertexDistance2 = distance(gEyeWorldPos, passPosition_e[2].xyz);

    // Calculate the tessellation levels
    gl_TessLevelOuter[0] = GetTessLevel(EyeToVertexDistance1, EyeToVertexDistance2);
    gl_TessLevelOuter[1] = GetTessLevel(EyeToVertexDistance2, EyeToVertexDistance0);
    gl_TessLevelOuter[2] = GetTessLevel(EyeToVertexDistance0, EyeToVertexDistance1);
    gl_TessLevelInner[0] = gl_TessLevelOuter[2];
}