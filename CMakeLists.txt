cmake_minimum_required(VERSION 3.0)

project(chonkerdefence LANGUAGES C)

add_definitions(
    -DAPP_TITLE="Chonker Defence"
    -DAPP_ENGEINE="SDL2"
    -DAPP_WIDTH=1536
    -DAPP_HEIGHT=864
    -DASSET_PATH="/home/david/Programmieren/C/ChonkerDefenceAssets/assets/"
    -DCDNET_PORT=42069
)

file(GLOB SRC_FILES
    "*.c"
    "src/*.c"
    "src/menu/*.c"
    "src/meta/*.c"
)

#SDL2
find_package(SDL2 REQUIRED)
if (NOT SDL2_FOUND)
    message (FATAL_ERROR "SDL not found")
else()
    include_directories (${SDL2_INCLUDE_DIRS})
    link_directories (${SDL2_LIBRARIES})
endif ()

#SDL_IMAGE
add_library(sdl2_image SHARED IMPORTED) # or STATIC instead of SHARED
set_target_properties(sdl2_image PROPERTIES
  IMPORTED_LOCATION "/usr/lib/x86_64-linux-gnu/libSDL2_image-2.0.so"
  INTERFACE_INCLUDE_DIRECTORIES "/usr/include/SDL2"
)
#SDL_FONTS
add_library(sdl2_ttf SHARED IMPORTED) # or STATIC instead of SHARED
set_target_properties(sdl2_ttf PROPERTIES
  IMPORTED_LOCATION "/usr/lib/x86_64-linux-gnu/libSDL2_ttf-2.0.so"
  INTERFACE_INCLUDE_DIRECTORIES "/usr/include/SDL2"
)

add_executable(${PROJECT_NAME} ${SRC_FILES})
target_link_libraries(${PROJECT_NAME} SDL2)
target_link_libraries(${PROJECT_NAME} sdl2_image)
target_link_libraries(${PROJECT_NAME} sdl2_ttf)

install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin)
