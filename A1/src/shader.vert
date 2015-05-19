#version 330

in vec3 vert;
in vec3 colour;
uniform mat4 mvpMatrix;

out vec4 colourV;

void main()
{
  colourV = vec4(colour, 1.0);
  gl_Position = mvpMatrix * vec4(vert, 1.0);
}
