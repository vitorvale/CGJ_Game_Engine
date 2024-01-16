#version 330
out vec4 colorOut;

in vec2 TexCoords;

uniform sampler2D image;

uniform float blur_kernel[9] = float[] (1.0 / 16.0, 2.0 / 16.0, 1.0 / 16.0,
        2.0 / 16.0, 4.0 / 16.0, 2.0 / 16.0,
        1.0 / 16.0, 2.0 / 16.0, 1.0 / 16.0);

uniform float offsets_x[9] = float[] (-1.0 / 300.0, 0.0, 1.0 / 300.0,
        -1.0 / 300.0, 0.0, 1.0 / 300.0,
        -1.0 / 300.0, 0.0, 1.0 / 300.0);

uniform float offsets_y[9] = float[] (1.0 / 300.0, 1.0 / 300.0, 1.0 / 300.0,
        0.0, 0.0, 0.0,
        -1.0 / 300.0, -1.0 / 300.0, -1.0 / 300.0);

const float gamma = 2.2;

void main()
{             
    colorOut = vec4(0.0);

    vec3 sample[9];
    // sample from texture offsets if using convolution matrix

    for(int i = 0; i < 9; i++)
        sample[i] = vec3(texture(image, TexCoords.st + vec2(offsets_x[i], offsets_y[i])));



    for(int i = 0; i < 9; i++)
        colorOut += vec4(sample[i] * blur_kernel[i], 0.0);
    colorOut.a = 1.0;

    colorOut.rgb = pow(colorOut.rgb, vec3(1.0/gamma)); //gamma correction

   

     
}