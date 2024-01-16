#version 330
in vec3 position;
in vec2 texCoord;

out vec2 TexCoords;

uniform float time;

void main()
{
    TexCoords = texCoord;
      
    gl_Position = vec4(position, 1.0);
    
}