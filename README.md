# Uppsala University - Computer Graphics Labs

## Lab 1: Basic Model Viewer Implementation
The first lab focuses on implementing a basic 3D model viewer using OpenGL. Key features:

- Basic window setup and OpenGL context initialization
- Model loading from GLTF files
- Simple rendering pipeline
- Mouse-based trackball camera control
- Shader program management

### Key Components:
```cpp
// Core initialization and window setup
glfwInit();
glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

// Basic rendering setup
glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
```

## Lab 2: Advanced Rendering Features
The second lab extends the model viewer with advanced features:

### New Features:
1. Lighting System
   - Ambient, diffuse, and specular lighting
   - Adjustable light position
   - Lighting enable/disable toggles

2. Material Properties
   - Color controls for different material components
   - Specular power adjustment
   - Normal visualization option

3. View Controls
   - Perspective/Orthographic projection toggle
   - Zoom functionality using scroll wheel
   - Enhanced camera controls

4. User Interface
   - ImGui-based control panels
   - Color pickers
   - Toggle switches for features

### Key Implementation Highlights:
```cpp
// Lighting uniforms
glUniform3f(glGetUniformLocation(ctx.program, "u_diffuseColor"), diffuseColor.x, diffuseColor.y, diffuseColor.z);
glUniform3f(glGetUniformLocation(ctx.program, "u_lightPosition"), lightPosition.x, lightPosition.y, lightPosition.z);

// Projection matrix setup
if (usePerspectiveProjection) {
    float fov = 45.0f * ctx.zoomFactor;
    projection = glm::perspective(glm::radians(fov), aspectRatio, nearPlane, farPlane);
} else {
    projection = glm::ortho(left, right, bottom, top, nearPlane, farPlane);
}
```

### Major Changes from Lab 1 to Lab 2:
1. Added global variables for material and lighting control
2. Implemented ImGui-based user interface
3. Enhanced projection matrix handling
4. Added zoom functionality
5. Introduced lighting calculations
6. Added material property controls

### Technical Improvements:
- Enhanced shader uniform management
- Added view matrix transformations
- Improved model transformation controls
- Implemented advanced rendering features

### Usage:
- Left mouse button: Rotate model
- Scroll wheel: Zoom in/out
- ImGui panels: Adjust material and lighting properties
- 'R' key: Reload shaders
- 
