#version 330
#extension GL_ARB_explicit_attrib_location : require

// Uniform constants
uniform float u_time;

// ...

// Vertex inputs (attributes from vertex buffers)
layout(location = 0) in vec4 a_position;
// ...

// Vertex shader outputs
// ...

void main()
{
    float radius = 0.3; // Set your desired radius
    float angularSpeed = 1; // Set your desired angular speed

    float angle = u_time * angularSpeed;
    vec2 circularPosition = vec2(cos(angle) * radius, sin(angle) * radius);

    vec2 newPosition = a_position.xy + circularPosition;

    gl_Position = vec4(newPosition, a_position.z, 1.0);
}
