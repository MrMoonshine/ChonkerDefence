#include <stdio.h>
#include <stdbool.h>
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

    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
    //glfwSetWindowSizeCallback(window, &window_resize_callback);
    glfwSetFramebufferSizeCallback(window, &framebuffer_size_callback);

    static const GLfloat g_vertex_buffer_data[] = {
    -1.0f, -1.0f, 0.0f,
    1.0f, -1.0f, 0.0f,
    0.0f,  1.0f, 0.0f,
    };

    GLuint VertexArrayID;
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);

    GLuint vertexbuffer;
    // Generate 1 buffer, put the resulting identifier in vertexbuffer
    glGenBuffers(1, &vertexbuffer);
    // The following commands will talk about our 'vertexbuffer' buffer
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    // Give our vertices to OpenGL.
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

    GLuint programID = glshader_load("../shaders/labver.glsl", "../shaders/labfra.glsl");
    //printf("Shader ID is %d\n", programID);
    glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    mat4 projection;
    float angle = 45.0f;
    glm_make_rad(&angle);
    glm_ortho(-10.0f, 10.0f, -10.0f, 10.0f, -10.0f, 10.0f, projection);

    mat4 view;
    vec3 eye = {4, 3, 3};
    vec3 center = {0, 0, 0};
    vec3 up = {0, 1, 0};
    glm_lookat(eye, center, up, view);
    //dumpMat4(view, "View");

    mat4 model;
    glm_mat4_identity(model);

    GLuint sProjection = glGetUniformLocation(programID, "projection");
    GLuint sView = glGetUniformLocation(programID, "view");
    GLuint sModel = glGetUniformLocation(programID, "model");

    bool server_running = false;
    server_initial_params serverparams = {
        .port = 0
    };
    Client client;
    ClientLevel level;


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
        glBindVertexArray(VertexArrayID);
        glUseProgram(programID);

        glUniformMatrix4fv(sProjection, 1, GL_FALSE, *projection);
        glUniformMatrix4fv(sView, 1, GL_FALSE, *view);
        glUniformMatrix4fv(sModel, 1, GL_FALSE, *model);

        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
        glVertexAttribPointer(
        0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
        3,                  // size
        GL_FLOAT,           // type
        GL_FALSE,           // normalized?
        0,                  // stride
        (void*)0            // array buffer offset
        );

        // Draw the triangle !
        glDrawArrays(GL_TRIANGLES, 0, 3); // Starting from vertex 0; 3 vertices total -> 1 triangle
        glDisableVertexAttribArray(0);

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
                            // Loading level
                            printf("Main: Loading Level #%d\n", levelID);
                            size_t len = 0;
                            clilevel_get_level(&client, &len, levelID, &level);
                            printf("Level: %s, style: %s\tdimensions are (%d|%d)\n", level.name, level.style, level.width, level.height);
                            currentMenu = MENU_GAME;
                            ui_levelmenu_destroy(&levelmenu);
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

    } // Check if the ESC key was pressed or the window was closed
    while(/*glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS && */glfwWindowShouldClose(window) == 0 && !mainLoopClosing);

    // Cleanup VBO and shader
	glDeleteBuffers(1, &vertexbuffer);
	glDeleteProgram(programID);
	glDeleteVertexArrays(1, &VertexArrayID);

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
        default: break;
    }

    // ui_mainmenu_destroy(&mainmenu);
    // levelmenu_destroy(&menu);
    ui_destroy(&ui);

    glfwDestroyCursor(cursor_default);
    glfwDestroyCursor(cursor_pointer);
    glfwDestroyWindow(window);
	// Close OpenGL window and terminate GLFW
	glfwTerminate();

    //client_server_kill(&client, serverparams.sessionkey, serverparams.thrid);
    return 0;
}
