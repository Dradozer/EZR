cmake_minimum_required(VERSION 3.0)
project(Glitter)

option(GLFW_BUILD_DOCS OFF)
option(GLFW_BUILD_EXAMPLES OFF)
option(GLFW_BUILD_TESTS OFF)
add_subdirectory(Glitter/Vendor/glfw)

#option(ASSIMP_BUILD_ASSIMP_TOOLS OFF)
#option(ASSIMP_BUILD_SAMPLES OFF)
#option(ASSIMP_BUILD_TESTS OFF)
#add_subdirectory(Glitter/Vendor/assimp)

#option(BUILD_BULLET2_DEMOS OFF)
option(BUILD_CPU_DEMOS OFF)
option(BUILD_EXTRAS OFF)
option(BUILD_OPENGL3_DEMOS OFF)
option(BUILD_UNIT_TESTS OFF)
#add_subdirectory(Glitter/Vendor/bullet)

if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic -std=c++11")
    if(NOT WIN32)
        set(GLAD_LIBRARIES dl)
    endif()
endif()

file(COPY Glitter/Resources DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

include_directories(Glitter/Headers/
#                    Glitter/Vendor/assimp/include/
#                    Glitter/Vendor/bullet/src/
                    Glitter/Vendor/glad/include/
                    Glitter/Vendor/glfw/include/
                    Glitter/Vendor/glm/
                    Glitter/Vendor/stb/)

file(GLOB VENDORS_SOURCES Glitter/Vendor/glad/src/glad.c)
file(GLOB PROJECT_HEADERS Glitter/Headers/*.hpp
						  Glitter/Headers/*.h)
file(GLOB PROJECT_SOURCES Glitter/Sources/*.cpp)
file(GLOB PROJECT_SHADERS Glitter/Shaders/*.comp
                          Glitter/Shaders/*.frag
                          Glitter/Shaders/*.geom
                          Glitter/Shaders/*.vert
                          Glitter/Shaders/*.glsl)
file(GLOB PROJECT_CONFIGS CMakeLists.txt
                          Readme.md
                         .gitattributes
                         .gitignore
                         .gitmodules)

source_group("Headers" FILES ${PROJECT_HEADERS})
source_group("Shaders" FILES ${PROJECT_SHADERS})
source_group("Sources" FILES ${PROJECT_SOURCES})
source_group("Vendors" FILES ${VENDORS_SOURCES})

add_definitions(-DGLFW_INCLUDE_NONE
                -DPROJECT_SOURCE_DIR=\"${PROJECT_SOURCE_DIR}\")
add_executable(${PROJECT_NAME} ${PROJECT_SOURCES} ${PROJECT_HEADERS}
                               ${PROJECT_SHADERS} ${PROJECT_CONFIGS}

                               ${VENDORS_SOURCES} Glitter/Sources/EZR_Shader/ezr_shader.cpp
                                Glitter/Headers/ezr_shader.h Glitter/Sources/EZR_Shader/ezr_ssbo.h Glitter/Headers/ezr_computeShader.h
                                Glitter/Sources/GUI/GUI.cpp Glitter/Sources/GUI/include/GUI.h Glitter/Sources/GUI/include/camera.h Glitter/Sources/GUI/Window.cpp
                                Glitter/Sources/GUI/include/Window.h Glitter/Sources/GUI/baseObject.cpp Glitter/Sources/GUI/include/baseObject.h Glitter/Sources/GUI/include/sceneInfo.h
                                Glitter/Sources/GUI/exampleObject.cpp Glitter/Sources/GUI/include/exampleObject.h Glitter/Sources/TextureDebug.cpp Glitter/Headers/TextureDebug.h Glitter/Sources/Texture.cpp Glitter/Headers/Texture.h
                                Glitter/Sources/Texture.cpp Glitter/Sources/ComputeShader.cpp Glitter/Headers/ComputeShader.h Glitter/Sources/Sky.cpp Glitter/Headers/Sky.h
                                Glitter/Sources/EZR_Shader/ezr_shader.cpp Glitter/Sources/EZR_Shader/ezr_ssbo.h Glitter/Sources/GUI/GUI.cpp Glitter/Sources/GUI/include/GUI.h Glitter/Sources/GUI/include/camera.h Glitter/Sources/GUI/Window.cpp Glitter/Sources/GUI/include/Window.h Glitter/Sources/GUI/baseObject.cpp Glitter/Sources/GUI/include/baseObject.h Glitter/Sources/GUI/include/sceneInfo.h Glitter/Sources/GUI/exampleObject.cpp Glitter/Sources/GUI/include/exampleObject.h Glitter/Sources/Terrain.cpp Glitter/Headers/Terrain.h Glitter/Sources/Chunk.cpp Glitter/Headers/Chunk.h Glitter/Headers/Vertex.h Glitter/Headers/cmpVecs.h Glitter/Headers/Texture.h Glitter/Sources/Texture.cpp Glitter/Sources/utils.cpp Glitter/Headers/ScreenQuad.h Glitter/Sources/ScreenQuad.cpp Glitter/Sources/frameBufferObject.cpp Glitter/Headers/frameBufferObject.h Glitter/Headers/TextureDebug.h Glitter/Sources/TextureDebug.cpp)

target_link_libraries(${PROJECT_NAME}
#        assimp
        glfw
                      ${GLFW_LIBRARIES} ${GLAD_LIBRARIES}
#                      BulletDynamics BulletCollision LinearMath
        )
set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${PROJECT_NAME})

add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/Glitter/Shaders $<TARGET_FILE_DIR:${PROJECT_NAME}>
    DEPENDS ${PROJECT_SHADERS})
