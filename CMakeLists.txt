cmake_minimum_required(VERSION 3.20)

project(chonkerdefence3d LANGUAGES C)

add_definitions(
    -DAPP_TITLE="Chonker Defence"
    -DAPP_ENGEINE="OpenGL"
    -DAPP_WIDTH=1024
    -DAPP_HEIGHT=720
    -DASSET_PATH="/home/david/Programmieren/C/ChonkerDefence/assets/"
    -DCDNET_PORT=42069
    -DOpenGL_GL_PREFERENCE=GLVND
)

file(GLOB SRC_FILES
    "*.c"
    "src/*.c"
    "src/ui/*.c"
    "./lib/fast_obj/*.c"
)

add_compile_options(-Wall)

add_executable(${PROJECT_NAME} ${SRC_FILES})

find_package(OpenGL REQUIRED)
find_package(glfw3 3.3 REQUIRED)
find_package(GLEW REQUIRED)
find_package(PNG REQUIRED)

# CGLM: https://github.com/recp/cglm.git
add_subdirectory("lib/cglm/")
# Fast OBJ: https://github.com/thisistherk/fast_obj.git
add_subdirectory("lib/fast_obj/")

set(INC_DIR
    "./inc"
    "./lib/cglm/include"
    "./lib/fast_obj"
    ${PNG_INCLUDE_DIR}  # Lib PNG
)
include_directories(${INC_DIR})

set(ALL_LIBS
    ${CMAKE_DL_LIBS}
	${OPENGL_LIBRARY}
	glfw
	GLEW::GLEW
	${PNG_LIBRARY}
)
target_link_libraries(${PROJECT_NAME} ${ALL_LIBS} -lm)

install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin)
