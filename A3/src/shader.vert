#version 330

in vec3 vert;
in vec3 norm;

uniform mat4 mvpMatrix;
uniform mat4 mvMatrix;
uniform mat3 normMatrix;
uniform vec3 lightPosition;

out vec3 vertNorm;
out vec3 lightDirection;
out vec3 viewerDirection;

void main()
{
    vec4 vertex = vec4(vert, 1.0);
    vec4 eyeVertex = mvMatrix * vertex;
    eyeVertex /= eyeVertex.w;

    vertNorm = normMatrix * norm;
    lightDirection = lightPosition - eyeVertex.xyz;
    viewerDirection = -eyeVertex.xyz;

    gl_Position = mvpMatrix * vertex;
}
