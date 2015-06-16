#version 330

uniform vec3 colour;

out vec4 finalColour;

void main()
{
    finalColour = vec4(colour, 1);
}
