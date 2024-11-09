#version 330
#extension GL_ARB_explicit_attrib_location : require

// Uniform constants
uniform float u_time;

// Fragment shader inputs
// ...

// Fragment shader outputs
out vec4 frag_color;

void main()
{
    // Use u_time to change the color over time
    float pulse = 0.5 + 0.2 * sin(u_time);

    // Set the red, green, and blue components of the color
    vec3 dynamicColor = vec3(pulse, 0.1, 0.1);

    // Output the final color
    frag_color = vec4(dynamicColor, 1.0);
}
