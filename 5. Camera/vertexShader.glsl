#version 330 core

layout (location = 0) in vec3 Position;
layout (location = 1) in vec3 Color;

out vec3 outColor;

uniform mat4 ModelMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;

void main()
{
    gl_Position = ProjectionMatrix * ViewMatrix * ModelMatrix * vec4(Position, 1.0);
    outColor = Color;
}
