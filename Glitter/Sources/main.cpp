// Local Headers
#include "glitter.hpp"
#include "GUI/include/camera.h"
#include "GUI/include/Window.h"
#include "GUI/include/GUI.h"
#include "GUI/include/sceneInfo.h"
#include "GUI/include/baseObject.h"
#include "noiseGenerator.h"

// BaseObjects
#include "GUI/include/exampleObject.h"
#include "Terrain.h"

// System Headers
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

// Shader include
#include "EZR_Shader/ezr_shader.h"
#include "EZR_Shader/ezr_ssbo.h"
#include "EZR_Shader/ezr_computeShader.h"

// Standard Headers
#include <cstdio>
#include <cstdlib>

int main(int argc, char *argv[]) {

    glm::vec3 startPosition(70.0f, 120.0f, 100.0f);
    Camera camera(startPosition);

    int success;
    Window window(success, 1000, 800, "EZR");
    if (!success) return -1;

    window.camera = &camera;
    GUI gui(window);
    glm::vec3 lightPosition, seed;
    lightPosition = glm::vec3(0.f);
    seed = glm::vec3(0.f);

    sceneInfo scene;
    scene.fogColor = glm::vec3(0.5, 0.6, 0.7);
    scene.lightColor = glm::vec3(255, 255, 230) / 255.f;;
    scene.lightDir = glm::vec3(-.5, 0.5, 1.0);
    scene.cam = &camera;
    scene.lightPos = lightPosition;
    scene.seed = seed;
    scene.projMatrix = glm::perspective(glm::radians(camera.Zoom),
                                        window.SCR_HEIGHT*1.f / window.SCR_HEIGHT*1.f, 0.1f, 1000000.0f);
    baseObject::scene = &scene;
    exampleObject example;
    gui.subscribe(&example);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    Terrain terrain(&window, &scene);

    // temporary noise generation example
    noiseGenerator noiseGenerator;
    GLuint noiseTexture0 = noiseGenerator.noiseTexture2D(128); // generate 128x128 2D perlin noise
    noiseGenerator.noiseEngine->SetNoiseType(FastNoise::Simplex); // change noise type to simplex
    GLuint noiseTexture1 = noiseGenerator.noiseTexture3D(64); // generate 64x64x64 3D simplex noise

    glfwSwapInterval(false);

    // Rendering Loop
    while (window.continueLoop())
    {

        glClearColor(0.25f, 0.25f, 0.25f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        auto frame_time = 1 / ImGui::GetIO().Framerate;
        window.processInput(frame_time);

        gui.update();
        terrain.render();

        // Flip Buffers and Draw

        gui.draw();
        window.swapBuffersAndPollEvents();
    }

    GLenum glError;
    if ((glError = glGetError()) != GL_NO_ERROR) {
        std::cout << "Program ende OpenGL-Error: " << glError << std::endl;
    }
    glfwTerminate();


    return EXIT_SUCCESS;
}