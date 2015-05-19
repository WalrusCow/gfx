#version 330

in vec3 vert;
in vec3 color;
uniform mat4 mvpMatrix;

out vec4 colorV;

void main()
{
  colorV = vec4(color, 1.0);
  gl_Position = mvpMatrix * vec4(vert, 1.0);
}
