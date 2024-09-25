#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aColor;  // Nouvel attribut pour la couleur

out vec3 ourColor;  // Passe la couleur au fragment shader

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    ourColor = aColor;  // Transmet la couleur au fragment shader
}
