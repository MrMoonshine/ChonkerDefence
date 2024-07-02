#include <ui/mainmenu.h>

static const float BUTTON_GAP = 16.0f;
static const float BUTTON_HEIGHT = 64.0f;
static const float BUTTON_WIDTH_WIDE = 512.0f;
static const float BUTTON_WIDTH = (BUTTON_WIDTH_WIDE - BUTTON_GAP) / 2.0f;
//static const float BUTTON_SPACE_HEIGHT = (BUTTON_HEIGHT + BUTTON_GAP) * 4 + BUTTON_GAP;
static const vec4 TITLE_COLOR = {0.82, 0.22, 0.42, 1.0};

int ui_mainmenu_create(MainMenu* menu, UI* ui){
    menu->ui = ui;
    //Title
    //ui_text2d_create(&selection->title, ui, "the quick brown fox jumps\r\nover the lazy dog!", 64.0f);
    ui_text2d_create(&menu->title, menu->ui, APP_TITLE, 56.0f);
    ui_text2d_set_color(&menu->title, TITLE_COLOR);

    //Background
    ui_image2d_create_from_png(&menu->background, ui, "../build/test8.png");

    //Buttons
    ui_button_create(&menu->btnSingleplayer, menu->ui,  BUTTON_WIDTH_WIDE,  BUTTON_HEIGHT, "Singleplayer");
    ui_button_create(&menu->btnJoin, menu->ui,          BUTTON_WIDTH_WIDE,  BUTTON_HEIGHT, "Join Game");
    ui_button_create(&menu->btnServer, menu->ui,        BUTTON_WIDTH_WIDE,  BUTTON_HEIGHT, "Host Server");
    ui_button_create(&menu->btnAbout, menu->ui,         BUTTON_WIDTH,       BUTTON_HEIGHT, "About");
    ui_button_create(&menu->btnQuit, menu->ui,          BUTTON_WIDTH,       BUTTON_HEIGHT, "Quit");

    return 0;
}

int ui_mainmenu_draw(MainMenu* menu){
    vec2 origin = {
        menu->ui->windowWidth/2 - BUTTON_WIDTH_WIDE/2,
        menu->ui->windowHeight/2// - BUTTON_SPACE_HEIGHT/2
    };

    ui_image2d_draw(&menu->background);

    menu->title.x = menu->ui->windowWidth/2 - menu->title.width/2;
    menu->title.y = origin[1] + BUTTON_HEIGHT + 2*BUTTON_GAP;
    ui_text2d_draw(&menu->title);

    int ret = 0;
    ui_button_set_position(&menu->btnSingleplayer, origin);
    if(ui_button_draw(&menu->btnSingleplayer)){
        ret = UI_MAINMENU_SINGLEPLAYER;
    }

    origin[1] -= BUTTON_HEIGHT + BUTTON_GAP;
    ui_button_set_position(&menu->btnJoin, origin);
    if(ui_button_draw(&menu->btnJoin)){
        printf("Join!\n");
    }

    origin[1] -= BUTTON_HEIGHT + BUTTON_GAP;
    ui_button_set_position(&menu->btnServer, origin);
    if(ui_button_draw(&menu->btnServer)){
        printf("Server!\n");
    }

    origin[1] -= BUTTON_HEIGHT + BUTTON_GAP;
    ui_button_set_position(&menu->btnAbout, origin);
    if(ui_button_draw(&menu->btnAbout)){
        printf("About!\n");
    }

    origin[0] += BUTTON_WIDTH + BUTTON_GAP;
    ui_button_set_position(&menu->btnQuit, origin);
    if(ui_button_draw(&menu->btnQuit)){
        ret = UI_MAINMENU_QUIT;
    }

    return ret;
}

void ui_mainmenu_destroy(MainMenu* menu){
    ui_button_destroy(&menu->btnSingleplayer);
    ui_button_destroy(&menu->btnJoin);
    ui_button_destroy(&menu->btnServer);
    ui_button_destroy(&menu->btnAbout);
    ui_button_destroy(&menu->btnQuit);
    ui_text2d_destroy(&menu->title);
    ui_image2d_destroy(&menu->background);
}
