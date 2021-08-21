// Local Headers
#include "glitter.hpp"
#include "GUI/include/camera.h"
#include "GUI/include/Window.h"
#include "GUI/include/GUI.h"
#include "GUI/include/sceneInfo.h"
#include "GUI/include/baseObject.h"
#include "NoiseGenerator.h"
#include "Cloud.h"
#include "TextureDebug.h"
#include "ComputeShader.h"
#include "Sky.h"
#include "Texture.h"

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
#include "ezr_shader.h"
#include "EZR_Shader/ezr_ssbo.h"
#include "ezr_computeShader.h"

// Standard Headers
#include <cstdio>
#include <math.h>
#include <cstdlib>
#include <utils.h>
#include <ScreenQuad.h>
#include <frameBufferObject.h>
#include <TextureDebug.h>

#define PI 3.1415926535

int main(int argc, char *argv[]) {

    glm::vec3 startPosition(70.0f, 120.0f, 100.0f);
    Camera camera(startPosition);
    GLenum glError;
    int success;
    Window window(success, 1000, 800, "EZR");
    if (!success) return -1;

    window.camera = &camera;
    GUI gui(window);
    glm::vec3 lightPosition, seed;
    lightPosition = glm::vec3(0.f);
    seed = glm::vec3(0.f);
    seed = genRandomVec3();
    frameBufferObject sceneFBO(window.SCR_WIDTH, window.SCR_HEIGHT, 2);
    if ((glError = glGetError()) != GL_NO_ERROR) {
        std::cout << "SceneFBO init OpenGL-Error: " << glError << std::endl;
    }
    sceneFBO.unbindCurrentFB();
    sceneFBO.bindTFBO();
    sceneInfo scene;
    scene.sceneFBO = &sceneFBO;
    scene.resolution.x = window.SCR_WIDTH;
    scene.resolution.y = window.SCR_HEIGHT;

    scene.fogColor = glm::vec3(0.5, 0.6, 0.7);
    scene.lightColor = glm::vec3(255, 255, 230) / 255.f;
    float aspect = (float) (Window::SCR_WIDTH) / (float) (Window::SCR_HEIGHT);
    std::cout << aspect << std::endl;
    scene.projMatrix = glm::perspective(glm::radians(camera.Zoom), aspect, 0.1f, 400.0f);

    scene.lightDir = glm::vec3(0.63, 0.78, 0.0);
    scene.cam = &camera;
    scene.cam->MovementSpeed = 20;
    scene.lightPos = scene.lightDir * 1e6f + camera.Position;
    scene.seed = seed;
    scene.projMatrix = glm::perspective(glm::radians(camera.Zoom),
                                        window.SCR_HEIGHT * 1.f / window.SCR_HEIGHT * 1.f, 0.1f, 1000000.0f);
    baseObject::scene = &scene;

    // create ShaderProgram
    GLuint m_renderID, m_debug;
    ezr::Shader::attachShader(m_renderID, GL_VERTEX_SHADER, "vertexShader.glsl");
    ezr::Shader::attachShader(m_renderID, GL_FRAGMENT_SHADER, "fragmentShader.glsl");
    ezr::Shader::linkShader(m_renderID);

    ezr::Shader::attachShader(m_debug, GL_VERTEX_SHADER, "compVert.glsl");
    ezr::Shader::attachShader(m_debug, GL_FRAGMENT_SHADER, "compFrag.glsl");
    ezr::Shader::linkShader(m_debug);

    int tex_w = window.SCR_WIDTH, tex_h = window.SCR_HEIGHT;

    Texture *textureGenerator = new Texture();
    GLuint tex_curr, tex_prev;
    tex_curr = textureGenerator->generate2DTexture(window.SCR_WIDTH, window.SCR_HEIGHT, GL_RGBA32F, GL_RGBA, GL_FLOAT);
    tex_prev = textureGenerator->generate2DTexture(window.SCR_WIDTH, window.SCR_HEIGHT, GL_RGBA32F, GL_RGBA, GL_FLOAT);

    const auto &skyBoxShader = new Sky(window.SCR_WIDTH, window.SCR_HEIGHT);
    const auto &clouds = new Clouds(Clouds::GPU);
    const auto &cloudShader = new ComputeShader(clouds->m_cloudTexture, clouds->m_detailTexture,
                                                clouds->m_motionTexture, clouds->m_weatherTexture,
                                                tex_curr, tex_prev, skyBoxShader->getTextureID(),
                                                clouds->m_blueNoiseTexture); 
    gui.subscribe(cloudShader);
    gui.subscribe(clouds);

    if ((glError = glGetError()) != GL_NO_ERROR) {
        std::cout << "Shader validation OpenGL-Error: " << glError << std::endl;
    }
    if ((glError = glGetError()) != GL_NO_ERROR) {
        std::cout << "Shader validation OpenGL-Error: " << glError << std::endl;
    }
    GLuint tex_output;
    glGenTextures(1, &tex_output);
    glBindTexture(GL_TEXTURE_2D, tex_output);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, tex_w, tex_h, 0, GL_RGBA, GL_FLOAT,
                 NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glGenerateMipmap(GL_TEXTURE_2D);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    Terrain terrain(&window, &scene);
    gui.subscribe(&terrain);

    //create somthing to render
//    const auto &screenSpaceShader = new ScreenQuad();
    if ((glError = glGetError()) != GL_NO_ERROR) {
        std::cout << "Shader validation OpenGL-Error: " << glError << std::endl;
    }

    //create somthing to render
    GLuint m_triagleID;
    glGenVertexArrays(1, &m_triagleID);
    glBindVertexArray(m_triagleID);

    static const GLfloat m_triagle[] = {
            -1.0f, -1.0f, 0.0f,
            1.0f, -1.0f, 0.0f,
            0.0f, 1.0f, 0.0f,
    };

    GLuint vertexbuffer;

//    TextureDebug textureDebug;
//    textureDebug.init(window.SCR_WIDTH, window.SCR_HEIGHT);
//    gui.subscribe(&textureDebug);

    GLuint m_uniform_resolution = glGetUniformLocation(m_debug, "resolution");

    GLuint m_screenSpaceID;
    //Screenspace shit
    ezr::Shader::attachShader(m_screenSpaceID, GL_VERTEX_SHADER, "screenVert.glsl");
    ezr::Shader::attachShader(m_screenSpaceID, GL_FRAGMENT_SHADER, "visualizeFboFrag.glsl");
    ezr::Shader::linkShader(m_screenSpaceID);

    float vertices[] = {
            -1.0f, -1.0f, 0.0, 0.0,
            1.0f, -1.0f, 1.0, 0.0,
            -1.0f, 1.0f, 0.0, 1.0,
            1.0f, 1.0f, 1.0, 1.0,
            -1.0f, 1.0f, 0.0, 1.0,
            1.0f, -1.0f, 1.0, 0.0
    };

    unsigned int quadVAO, quadVBO;
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *) 0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *) (2 * sizeof(float)));
    if ((glError = glGetError()) != GL_NO_ERROR) {
        std::cout << "Shader validation OpenGL-Error: " << glError << std::endl;
    }

    GLint drawFboId = 0, readFboId = 0;
    // Rendering Loop
    glfwSwapInterval(0);
    while (window.continueLoop()) {

        glClearColor(0.25f, 0.25f, 0.25f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        auto frame_time = 1 / ImGui::GetIO().Framerate;
        window.processInput(frame_time);

        gui.update();

        //first pass
        glGetIntegerv(GL_FRAMEBUFFER_BINDING, &drawFboId);

        scene.sceneFBO->bindTFBO();
        glGetIntegerv(GL_FRAMEBUFFER_BINDING, &drawFboId);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, scene.sceneFBO->terrainTex);
        glClearColor(0.0, 0.0, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);
        glEnable(GL_CULL_FACE);
        terrain.render(); 

        //second pass
        // Flip Buffers and Draw
        //scene.sceneFBO->bindCFBO();

        glm::mat4 view = scene.cam->GetViewMatrix();
        GLuint store_id;

//        textureDebug.clear();
        glGetIntegerv(GL_FRAMEBUFFER_BINDING, &drawFboId);
        glActiveTexture(GL_TEXTURE0);
        glDisable(GL_DEPTH_TEST);
        glClearColor(0.00f, 0.00f, 0.5f, 1.0f);
        if (clouds->m_runtime_change) {
            std::cout << "Generating new Texture " << std::endl;
            switch (clouds->m_enum) {
                case 0:
                    clouds->getIDTexture(Clouds::CLOUD);
                    //textureDebug.addTexture(store_id,512,false);
                case 1:
                    clouds->getIDTexture(Clouds::DETAIL);
                    //textureDebug.addTexture(store_id, 512, false);
                case 2:
                    clouds->getIDTexture(Clouds::WEATHER);
//                    textureDebug.addTexture(clouds->m_weatherTexture, 512, false);
                    cloudShader->m_weatherTex = clouds->m_weatherTexture;
            }
        }

        clouds->m_runtime_change = false;

        skyBoxShader->render();
        cloudShader->draw();

//        third pass to screen
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glGetIntegerv(GL_FRAMEBUFFER_BINDING, &drawFboId);
        glUseProgram(m_screenSpaceID);
        glUniform2fv(m_uniform_resolution, 1, glm::value_ptr(scene.resolution));
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, scene.sceneFBO->terrainTex);
        glBindVertexArray(quadVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);

        gui.draw();
        window.swapBuffersAndPollEvents();
    }

    if ((glError = glGetError()) != GL_NO_ERROR) {
        std::cout << "Program ende OpenGL-Error: " << glError << std::endl;
    }
    glfwTerminate();


    return EXIT_SUCCESS;
}