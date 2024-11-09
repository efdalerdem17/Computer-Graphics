// Model viewer code for the assignments in Computer Graphics 1TD388/1MD150.
//
// Modify this and other source files according to the tasks in the instructions.
//

#include "gltf_io.h"
#include "gltf_scene.h"
#include "gltf_render.h"
#include "cg_utils.h"
#include "cg_trackball.h"

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <algorithm>  
#include <cstdlib>
#include <iostream>

// Global variables for ImGui widgets
glm::vec3 backgroundColor = glm::vec3(0.2f, 0.3f, 0.6f);
glm::vec3 diffuseColor = glm::vec3(0.5f, 0.5f, 0.5f);
glm::vec3 lightPosition = glm::vec3(0.0f, 1.0f, 2.0f);
glm::vec3 ambientColor = glm::vec3(0.1f, 0.1f, 0.1f);   // Default ambient color
glm::vec3 specularColor = glm::vec3(1.0f, 1.0f, 1.0f);  // Default specular color
float specularPower = 32.0f;                            // Default specular power
bool enableSpecular = true;
bool enableLighting = true;
bool enableAmbient = true;
bool displayNormals = false;
bool usePerspectiveProjection = true;


// Struct for our application context
struct Context {
    int width = 512;
    int height = 512;
    GLFWwindow *window;
    gltf::GLTFAsset asset;
    gltf::DrawableList drawables;
    cg::Trackball trackball;
    GLuint program;
    GLuint emptyVAO;
    float elapsedTime;
    std::string gltfFilename = "armadillo.gltf";
    float zoomFactor = 1.0f; 
    // Add more variables here...
};

// Returns the absolute path to the src/shader directory
std::string shader_dir(void)
{
    std::string rootDir = cg::get_env_var("MODEL_VIEWER_ROOT");
    if (rootDir.empty()) {
        std::cout << "Error: MODEL_VIEWER_ROOT is not set." << std::endl;
        std::exit(EXIT_FAILURE);
    }
    return rootDir + "/src/shaders/";
}

// Returns the absolute path to the assets/gltf directory
std::string gltf_dir(void)
{
    std::string rootDir = cg::get_env_var("MODEL_VIEWER_ROOT");
    if (rootDir.empty()) {
        std::cout << "Error: MODEL_VIEWER_ROOT is not set." << std::endl;
        std::exit(EXIT_FAILURE);
    }
    return rootDir + "/assets/gltf/";
}

void do_initialization(Context &ctx)
{
    ctx.program = cg::load_shader_program(shader_dir() + "mesh.vert", shader_dir() + "mesh.frag");

    gltf::load_gltf_asset(ctx.gltfFilename, gltf_dir(), ctx.asset);
    gltf::create_drawables_from_gltf_asset(ctx.drawables, ctx.asset);
}

void draw_scene(Context &ctx)
{
    glm::mat4 view = glm::mat4(ctx.trackball.orient);
    glm::mat4 projection = glm::mat4(1.0f);
    glm::mat4 model = glm::mat4(1.0f);

    // glm::lookAt
    glm::vec3 eye = glm::vec3(0.0f, 0.0f, 3.0f);       
    glm::vec3 at = glm::vec3(0.0f, 0.0f, 0.0f);    
    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);              
    view = glm::lookAt(eye, at, up) * view;  // combining it with the trackball matrix.

    // Toggle between perspective and orthographic projection
    if (usePerspectiveProjection) {
        float fov = 45.0f * ctx.zoomFactor;
        float aspectRatio = ctx.width / static_cast<float>(ctx.height);
        float nearPlane = 0.1f;
        float farPlane = 10.0f;
        projection = glm::perspective(glm::radians(fov), aspectRatio, nearPlane, farPlane);
    } else {
        float left = -2.5f;
        float right = 2.5f;
        float bottom = -2.5f;
        float top = 2.5f;
        float nearPlane = 0.1f;
        float farPlane = 3.5f;
        projection = glm::ortho(left, right, bottom, top, nearPlane, farPlane);
    }

    model = glm::translate(model, glm::vec3(0.1f, 0.1f, 0.0f));  
    model = glm::scale(model, glm::vec3(0.7f));                 
    model = glm::rotate(model, glm::radians(150.0f), glm::vec3(0.0f, 0.5f, 0.0f));  

    // Activate shader program
    glUseProgram(ctx.program);

    // Set render state
    glEnable(GL_DEPTH_TEST);  // Enable Z-buffering

    // Define per-scene uniforms
    glUniform1f(glGetUniformLocation(ctx.program, "u_time"), ctx.elapsedTime);
    glUniformMatrix4fv(glGetUniformLocation(ctx.program, "u_view"), 1, GL_FALSE, &view[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(ctx.program, "u_projection"), 1, GL_FALSE,
                       &projection[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(ctx.program, "u_model"), 1, GL_FALSE,
                       &model[0][0]);
    glUniform3f(glGetUniformLocation(ctx.program, "u_diffuseColor"), diffuseColor.x, diffuseColor.y, diffuseColor.z);
    glUniform3f(glGetUniformLocation(ctx.program, "u_lightPosition"), lightPosition.x, lightPosition.y, lightPosition.z);
    glUniform3f(glGetUniformLocation(ctx.program, "u_ambientColor"), ambientColor.x, ambientColor.y, ambientColor.z);
    glUniform3f(glGetUniformLocation(ctx.program, "u_specularColor"), specularColor.x, specularColor.y, specularColor.z);
    glUniform1f(glGetUniformLocation(ctx.program, "u_specularPower"), specularPower);
    
    glUniform1i(glGetUniformLocation(ctx.program, "u_enableSpecular"), enableSpecular ? 1 : 0);
    glUniform1i(glGetUniformLocation(ctx.program, "u_enableLighting"), enableLighting ? 1 : 0);
    glUniform1i(glGetUniformLocation(ctx.program, "u_enableAmbient"), enableAmbient ? 1 : 0);
    glUniform1i(glGetUniformLocation(ctx.program, "u_displayNormalsAsColors"), displayNormals ? 1 : 0);

    // Draw scene
    for (unsigned i = 0; i < ctx.asset.nodes.size(); ++i) {
        const gltf::Node &node = ctx.asset.nodes[i];
        const gltf::Drawable &drawable = ctx.drawables[node.mesh];

        // Define per-object uniforms
        // ...

        // Draw object
        glBindVertexArray(drawable.vao);
        glDrawElements(GL_TRIANGLES, drawable.indexCount, drawable.indexType,
                       (GLvoid *)(intptr_t)drawable.indexByteOffset);
        glBindVertexArray(0);
    }

    // Clean up
    cg::reset_gl_render_state();
    glUseProgram(0);
}

void do_rendering(Context &ctx)
{
    // Clear render states at the start of each frame
    cg::reset_gl_render_state();
    // Clear color and depth buffers
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);


    if (ImGui::TreeNode("Lighting")) {
        ImGui::InputFloat3("Light position", &lightPosition[0]);
        ImGui::Checkbox("Enable Lighting", &enableLighting);
        ImGui::TreePop();
    }

    if (ImGui::TreeNode("Material")) {
        ImGui::ColorEdit3("Diffuse color", &diffuseColor[0]);
        ImGui::ColorEdit3("Ambient color", &ambientColor[0]);
        ImGui::Checkbox("Enable Ambient", &enableAmbient);
        ImGui::ColorEdit3("Specular color", &specularColor[0]);
        ImGui::SliderFloat("Specular power", &specularPower, 1.0f, 100.0f);
        ImGui::Checkbox("Enable Specular", &enableSpecular);
        ImGui::TreePop();
    }
    if (ImGui::TreeNode("Background")) {
        ImGui::ColorEdit3("Background color", &backgroundColor[0]);
        ImGui::TreePop();
    }
    if (ImGui::TreeNode("RGB Colors")) {
		ImGui::Checkbox("Display normals", &displayNormals);
		ImGui::TreePop();
	}
    if (ImGui::TreeNode("Projection")) {
		ImGui::Checkbox("Perspective projection", &usePerspectiveProjection);
		ImGui::TreePop();
	}



    glClearColor(backgroundColor.r, backgroundColor.g, backgroundColor.b, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    draw_scene(ctx);
}

void reload_shaders(Context *ctx)
{
    glDeleteProgram(ctx->program);
    ctx->program = cg::load_shader_program(shader_dir() + "mesh.vert", shader_dir() + "mesh.frag");
}

void error_callback(int /*error*/, const char *description)
{
    std::cerr << description << std::endl;
}

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    // Forward event to ImGui
    ImGui_ImplGlfw_KeyCallback(window, key, scancode, action, mods);
    if (ImGui::GetIO().WantCaptureKeyboard) return;

    Context *ctx = static_cast<Context *>(glfwGetWindowUserPointer(window));
    if (key == GLFW_KEY_R && action == GLFW_PRESS) { reload_shaders(ctx); }
}

void char_callback(GLFWwindow *window, unsigned int codepoint)
{
    // Forward event to ImGui
    ImGui_ImplGlfw_CharCallback(window, codepoint);
    if (ImGui::GetIO().WantTextInput) return;
}

void mouse_button_callback(GLFWwindow *window, int button, int action, int mods)
{
    // Forward event to ImGui
    ImGui_ImplGlfw_MouseButtonCallback(window, button, action, mods);
    if (ImGui::GetIO().WantCaptureMouse) return;

    double x, y;
    glfwGetCursorPos(window, &x, &y);

    Context *ctx = static_cast<Context *>(glfwGetWindowUserPointer(window));
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        ctx->trackball.center = glm::vec2(x, y);
        ctx->trackball.tracking = (action == GLFW_PRESS);
    }
}

void cursor_pos_callback(GLFWwindow *window, double x, double y)
{
    // Forward event to ImGui
    if (ImGui::GetIO().WantCaptureMouse) return;

    Context *ctx = static_cast<Context *>(glfwGetWindowUserPointer(window));
    cg::trackball_move(ctx->trackball, float(x), float(y));
}

void scroll_callback(GLFWwindow *window, double x, double y)
{
    // Forward event to ImGui
    ImGui_ImplGlfw_ScrollCallback(window, x, y);
    if (ImGui::GetIO().WantCaptureMouse) return;

    // Get the context from the GLFW window user pointer
    Context *ctx = static_cast<Context *>(glfwGetWindowUserPointer(window));

    // Adjust the zoom factor based on the scroll direction
    constexpr float zoomSpeed = 0.05f;  // You can adjust the speed based on your preference
    ctx->zoomFactor *= (1.0f + zoomSpeed * static_cast<float>(y));

    // Clamp the zoom factor to prevent extreme values
    ctx->zoomFactor = glm::clamp(ctx->zoomFactor, 0.1f, 10.0f);
}

void resize_callback(GLFWwindow *window, int width, int height)
{
    // Update window size and viewport rectangle
    Context *ctx = static_cast<Context *>(glfwGetWindowUserPointer(window));
    ctx->width = width;
    ctx->height = height;
    glViewport(0, 0, width, height);
}

int main(int argc, char *argv[])
{
    Context ctx = Context();
    if (argc > 1) { ctx.gltfFilename = std::string(argv[1]); }

    // Create a GLFW window
    glfwSetErrorCallback(error_callback);
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    ctx.window = glfwCreateWindow(ctx.width, ctx.height, "Model viewer", nullptr, nullptr);
    glfwMakeContextCurrent(ctx.window);
    glfwSetWindowUserPointer(ctx.window, &ctx);
    glfwSetKeyCallback(ctx.window, key_callback);
    glfwSetCharCallback(ctx.window, char_callback);
    glfwSetMouseButtonCallback(ctx.window, mouse_button_callback);
    glfwSetCursorPosCallback(ctx.window, cursor_pos_callback);
    glfwSetScrollCallback(ctx.window, scroll_callback);
    glfwSetFramebufferSizeCallback(ctx.window, resize_callback);

    // Load OpenGL functions
    if (gl3wInit() || !gl3wIsSupported(3, 3) /*check OpenGL version*/) {
        std::cerr << "Error: failed to initialize OpenGL" << std::endl;
        std::exit(EXIT_FAILURE);
    }
    std::cout << "OpenGL version: " << glGetString(GL_VERSION) << std::endl;

    // Initialize ImGui
    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForOpenGL(ctx.window, false /*do not install callbacks*/);
    ImGui_ImplOpenGL3_Init("#version 330" /*GLSL version*/);

    // Initialize rendering
    glGenVertexArrays(1, &ctx.emptyVAO);
    glBindVertexArray(ctx.emptyVAO);
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
    do_initialization(ctx);

    // Start rendering loop
    while (!glfwWindowShouldClose(ctx.window)) {
        glfwPollEvents();
        ctx.elapsedTime = glfwGetTime();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        //ImGui::ShowDemoWindow();
        do_rendering(ctx);
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(ctx.window);
    }

    // Shutdown
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(ctx.window);
    glfwTerminate();
    std::exit(EXIT_SUCCESS);
}
