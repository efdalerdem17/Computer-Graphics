#version 330
#extension GL_ARB_explicit_attrib_location : require

// Uniform constants
uniform float u_time;
uniform mat4 u_view;
uniform mat4 u_projection;
uniform mat4 u_model;


// New uniforms for diffuse shading
uniform vec3 u_diffuseColor;    // The diffuse surface color of the model
uniform vec3 u_lightPosition;    // The position of your light source

// Material properties for Phong shading
uniform vec3 u_ambientColor;
uniform vec3 u_specularColor;
uniform float u_specularPower;

// Uniform to control specular reflection
uniform int u_enableLighting;
uniform int u_enableSpecular;
uniform int u_enableAmbient;
uniform int u_displayNormalsAsColors;

// Vertex inputs (attributes from vertex buffers)
layout(location = 0) in vec4 a_position;
layout(location = 1) in vec3 a_color;
layout(location = 2) in vec3 a_normal;

// Vertex shader outputs
out vec3 v_color;

void main()
{
    // Calculate view-space position
    vec4 positionEye = u_view * u_model * a_position;

    // Pass surface normals as RGB colors if enabled
    if (u_displayNormalsAsColors == 1) {
        v_color = (a_normal + 1.0) * 0.5; // Convert range [-1, 1] to [0, 1]

    } else {
        // Calculate view-space normal
        vec3 normalEye = normalize(mat3(u_view * u_model) * a_normal);

        // Calculate view-space light direction
        vec3 lightDirection = normalize(u_lightPosition - positionEye.xyz);

        // Calculate view-space half vector
        vec3 viewDirection = normalize(-positionEye.xyz);
        vec3 halfVector = normalize(lightDirection + viewDirection);

        // Calculate ambient term only if ambient is enabled
        vec3 ambient = vec3(0.0);
        if (u_enableAmbient == 1) {
            ambient = u_ambientColor * u_diffuseColor;
        }

        // Calculate diffuse term (Lambertian)
        float diffuse = max(0.0, dot(normalEye, lightDirection));

        // Calculate specular term (Blinn-Phong)
        float specular = 0.0;
        if (u_enableSpecular == 1) {
            specular = pow(max(dot(normalEye, halfVector), 0.0), u_specularPower);
        }

        vec3 finalColor = vec3(0.0);
        if (u_enableLighting == 1) {
            finalColor += ambient + diffuse * u_diffuseColor + specular * u_specularColor;
        } else {
            finalColor = u_diffuseColor; // Only use the diffuse color if light is disabled
        }
        // Assign the final color to v_color
        v_color = finalColor;
    }

    // Calculate final position
    gl_Position = u_projection * positionEye;
}
