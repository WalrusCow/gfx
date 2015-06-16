#version 330

in vec3 vert;

uniform mat4 mvpMatrix;

void main()
{
    gl_Position = mvpMatrix * vec4(vert, 1.0);
}
