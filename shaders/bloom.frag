#version 330
out vec4 colorOut;

in vec2 TexCoords;

uniform sampler2D scene;
uniform sampler2D bloomBlur;
bool bloom = true;
float exposure = 0.8;

void main()
{             
    const float gamma = 2.2;
    vec3 hdrColor = texture(scene, TexCoords).rgb;      
    vec3 bloomColor = texture(bloomBlur, TexCoords).rgb;
    if(bloom)
        hdrColor += bloomColor; // additive blending
    // tone mapping
    vec3 result = vec3(1.0) - exp(-hdrColor * exposure);
    // also gamma correct while we're at it       
    result = pow(result, vec3(1.0 / gamma));
    colorOut = vec4(result, 1.0);
}