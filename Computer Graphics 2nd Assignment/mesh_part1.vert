#version 330
#extension GL_ARB_explicit_attrib_location : require

// Uniform constants
uniform float u_time;
uniform mat4 u_view;
// ...

// Vertex inputs (attributes from vertex buffers)
layout(location = 0) in vec4 a_position;
layout(location = 1) in vec3 a_color;
layout(location = 2) in vec3 a_normal; // Add this line for the normal attribute


// ...

// Vertex shader outputs
// ...
out vec3 v_color;

void main()
{
	v_color = 0.5 * a_normal + 0.5;
	gl_Position = u_view * a_position;
}
