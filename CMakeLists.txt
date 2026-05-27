project("LAB01")

#Put the sources into a variable
set(SOURCES_EX_01 "main.cpp" "shader.h")

set(SOURCES_EX_ADD "main.cpp" "shader.h")


add_compile_definitions(PATH_TO_LAB1_SHADERS="${CMAKE_CURRENT_SOURCE_DIR}")

#To use the content of a variable you need to use ${NAME_OF_YOUR_VARIABLE}
#Specify that you want to generate an executable with a certain name using a set of sources
add_executable(${PROJECT_NAME}_ex01 ${SOURCES_EX_01} "main.cpp"  "camera.h" "util.h" "util.cpp" "tiny_obj_loader.h" "tiny_gltf.h" "effects/particle_system.h" "effects/particle_system.cpp" "effects/explosion.h"
    "effects/explosion.cpp" "effects/shadow.h" "effects/shadow.cpp" "shaders.h" "render.h" "render.cpp")
#Specify which libraries you want to use with your executable
target_link_libraries(${PROJECT_NAME}_ex01 PUBLIC OpenGL::GL glfw glad)

add_executable(${PROJECT_NAME}_ex22 ${SOURCES_EX_ADD} "main.cpp"  "camera.h" "util.h" "util.cpp" "tiny_obj_loader.h" "tiny_gltf.h"  "effects/particle_system.h" "effects/particle_system.cpp" "effects/explosion.h"
    "effects/explosion.cpp" "effects/shadow.h" "effects/shadow.cpp" "shaders.h" "render.h" "render.cpp")
target_link_libraries(${PROJECT_NAME}_ex22 PUBLIC OpenGL::GL glfw glad)