#version 330

uniform vec3 frag_colour;

uniform vec3 ambientColour;
uniform vec3 diffuseColour;
uniform vec3 specularColour;
uniform float shininess;

in vec3 vertNorm;
in vec3 lightDirection;
in vec3 viewerDirection;

out vec4 finalColour;

void main()
{
    vec3 normal = normalize(vertNorm);
    vec3 lightDirection = normalize(lightDirection);
    vec3 viewerDirection = normalize(viewerDirection);

    vec3 diffuseIllumination = max(0.0, dot(lightDirection, normal)) * diffuseColour;
    vec3 specularIllumination = pow(
      max(0.0, dot(-reflect(lightDirection, normal), viewerDirection)),
      shininess) * specularColour;
    vec3 newColour = frag_colour * (ambientColour + diffuseIllumination) + specularIllumination;
    finalColour = vec4(newColour, 1);
}
