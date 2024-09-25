#version 330 core

in vec3 ourColor;  // Reçoit la couleur interpolée depuis le vertex shader
out vec4 FragColor;

void main()
{
    FragColor = vec4(ourColor, 1.0);  // Applique la couleur interpolée
}
