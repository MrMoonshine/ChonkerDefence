#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include <stdlib.h>
#include <glshader.h>

#include <logging.h>
#include <server.h>
#include <client.h>
#include <clilevel.h>

#include <ui/ui.h>
#include <ui/mainmenu.h>
#include <ui/levelmenu.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <cglm/cglm.h>

static const char* TAG = "Main";

static UI ui;
static ClientLevel level;

// Used for the Menus as some sort of state machine
typedef enum {
    MENU_MAIN,
    MENU_LEVELSELECTION,
    MENU_GAME
} MenuState;

void framebuffer_size_callback(GLFWwindow* window, int width, int height){
    glViewport(0, 0, width, height);
    printf("[INFO] %s: My window size is %dx%d\n", TAG, width, height);
    ui_resize(&ui, width, height);
    clilevel_resize(&level, width, height);
}

void dumpMat4(mat4 matrix, const char* title){
    printf("-------- %s --------\n", title);
    for(int a = 0; a < 4; a++){
        for(int b = 0; b < 4; b++){
            printf("%.2f, ", matrix[a][b]);
        }
        printf("\n");
    }
    printf("------------------------\n");
}

int main(void){
    srand(time(NULL));
    //LOGS(TAG, "Starting");
    glewExperimental = true; // Needed for core profile
    if(!glfwInit()){
        LOGE(TAG, "Failed to initialize GLFW");
        return -1;
    }

    glfwWindowHint(GLFW_SAMPLES, 4); // 4x antialiasing
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // We want OpenGL 3.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window;
    window = glfwCreateWindow( APP_WIDTH, APP_HEIGHT, APP_TITLE, NULL, NULL);
    if(!window){
        LOGE(TAG, "Unsupported GPU in use!");
        glfwTerminate();
        return -1;
    }

    GLFWcursor* cursor_default = glfwCreateStandardCursor(GLFW_ARROW_CURSOR );
    GLFWcursor* cursor_pointer = glfwCreateStandardCursor(GLFW_HAND_CURSOR);
    glfwSetCursor(window, cursor_default);

    glfwMakeContextCurrent(window);
    if (glewInit() != GLEW_OK) {
        LOGE(TAG, "Failed to initialize GLEW");
        return -1;
    }
    // Show version
    printf("[INFO] %s: OpenGL version is %s\n", TAG, glGetString(GL_VERSION));
    printf("[INFO] %s: GLFW   version is %s\n", TAG, glfwGetVersionString());

    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
    //glfwSetWindowSizeCallback(window, &window_resize_callback);
    glfwSetFramebufferSizeCallback(window, &framebuffer_size_callback);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    bool server_running = false;
    server_initial_params serverparams = {
        .port = 0
    };
    Client client;
    // Main shader for the game
    GLuint gameShader = glshader_load("../shaders/gamever.glsl", "../shaders/gamefra.glsl");
    level.shader = gameShader;

    ui_create(&ui, window);

    MainMenu mainmenu;
    LevelMenu levelmenu;
    // Create initial Menu
    MenuState currentMenu = MENU_MAIN;
    ui_mainmenu_create(&mainmenu, &ui);

    bool mainLoopClosing = false;
    do{
        // Clear the screen. It's not mentioned before Tutorial 02, but it can cause flickering, so it's there nonetheless.
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if(currentMenu == MENU_GAME){
            clilevel_enable_vao(&level);
            clilevel_draw(&level);
        }

        ui_enable_vao(&ui);
        /*----------------------------------
                    Menus
        -----------------------------------*/
        switch(currentMenu){
            case MENU_MAIN:{
                switch(ui_mainmenu_draw(&mainmenu)){
                    case UI_MAINMENU_SINGLEPLAYER:
                        if(server_running){
                            LOGW(TAG, "Server already running!\n");
                        }else{
                            serverparams.port = 0;  // Singleplayer
                            server_start(&serverparams);
                            server_running = true;

                            client_init(&client, "::1", SERVER_PORT_DEFAULT);
                            // Switch menu
                            currentMenu = MENU_LEVELSELECTION;
                            ui_mainmenu_destroy(&mainmenu);
                            ui_levelmenu_create(&levelmenu, &ui, &client);
                        }
                        break;
                    case UI_MAINMENU_QUIT:
                        mainLoopClosing = true;
                        break;
                    default: break;
                }
            }break;
            case MENU_LEVELSELECTION:{
                int levelmenuaction = ui_levelmenu_draw(&levelmenu);
                switch(levelmenuaction){
                    case UI_LEVELMENU_BACK:{
                        if(server_running){
                            client_server_kill(&client, serverparams.sessionkey, serverparams.thrid);
                            server_running = false;
                        }
                        currentMenu = MENU_MAIN;
                        ui_levelmenu_destroy(&levelmenu);
                        ui_mainmenu_create(&mainmenu, &ui);
                    }break;
                    case 0:
                        break;
                    default: {
                        if(levelmenuaction > 0){
                            uint8_t levelID = levelmenuaction - 1;
                            ui_levelmenu_destroy(&levelmenu);
                            // Loading level
                            printf("[INFO] Main: Loading Level #%d\n", levelID);
                            size_t len = 0;
                            clilevel_get_level(&level, &client, window, &len, levelID);
                            clilevel_dump(&level);
                            currentMenu = MENU_GAME;
                        }
                    }break;
                }
            }break;
            default: break;
        }

        glfwSetCursor(window, ui.anyButtonHover ? cursor_pointer : cursor_default);

        // Swap buffers
        glfwSwapBuffers(window);
        glfwPollEvents();

    }while(/*glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS && */glfwWindowShouldClose(window) == 0 && !mainLoopClosing);

    if(server_running){
        client_server_kill(&client, serverparams.sessionkey, serverparams.thrid);
    }
    switch(currentMenu){
        case MENU_MAIN:
            ui_mainmenu_destroy(&mainmenu);
            break;
        case MENU_LEVELSELECTION:
            ui_levelmenu_destroy(&levelmenu);
            break;
        case MENU_GAME:
            clilevel_destroy(&level);
            break;
        default: break;
    }
    // ui_mainmenu_destroy(&mainmenu);
    // levelmenu_destroy(&menu);
    ui_destroy(&ui);
    glDeleteProgram(gameShader);

    printf("Here 1\n");
    glfwDestroyCursor(cursor_default);
    printf("Here 2\n");
    glfwDestroyCursor(cursor_pointer);
    printf("Here 3: %d\n", window == NULL);
    glfwDestroyWindow(window);
    printf("Here 4\n");
	// Close OpenGL window and terminate GLFW
	glfwTerminate();

    //client_server_kill(&client, serverparams.sessionkey, serverparams.thrid);
    return 0;
}
