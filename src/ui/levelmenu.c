#include <ui/levelmenu.h>

//static const char* TAG = "GUI: Levelmenu:";

//static const size_t BUFFER_SIZE = 1024;
static const uint8_t BUTTON_COLUMNS = 3;
static const float BUTTON_GAP = 16.0f;
static const float BUTTON_HEIGHT = 40.0f;
static const float BUTTON_WIDTH = APP_WIDTH/BUTTON_COLUMNS - BUTTON_GAP;
static const float TITLE_FONT_SIZE = 60.0f;
static const float BUTTON_CONTAINER_WIDTH = BUTTON_COLUMNS*(BUTTON_WIDTH + BUTTON_GAP) - BUTTON_GAP;

static int ui_levelmenu_build_buttons(LevelMenu *menu){
    size_t len = 0;
    uint8_t status = clilevel_get_info(menu->client, &len);
    if(status != CD_NET_CODE_OK || len < 1){
        return -1;
    }
    uint8_t *buffer;
    buffer = (uint8_t*)malloc(len);
    recv(menu->client->socket, buffer, len, 0);
    size_t pos = 0;
    // First byte is levelcount
    menu->buttonCount = buffer[pos++];
    menu->buttons = (Button*)malloc(menu->buttonCount * sizeof(Button));

    for(uint8_t i = 0; i < menu->buttonCount; i++){
        uint8_t slen = buffer[pos++];
        //printf("%s\tStrlen is %d\n", (char*)(buffer + pos), slen);
        ui_button_create(menu->buttons + i, menu->ui,  BUTTON_WIDTH,  BUTTON_HEIGHT, (char*)(buffer + pos));
        pos += slen + 1; // +1 for the \0
    }
    free(buffer);
    return 0;
}

int ui_levelmenu_create(LevelMenu *menu, UI* ui, Client* client){
    menu->ui = ui;
    menu->client = client;
    menu->buttonCount = 0;

    //Background
    int bgwidth = APP_WIDTH * 1.3 + 32, bgheight = APP_HEIGHT*1.2 + 32;
    ui_image2d_create_from_png(&menu->background, ui, "../build/test3.png");

    ui_button_create(&menu->btnBack, menu->ui,  BUTTON_HEIGHT*5,  BUTTON_HEIGHT*1.5f, "Back");

    ui_text2d_create(&menu->title, ui, "Levels", TITLE_FONT_SIZE);
    ui_levelmenu_build_buttons(menu);
    return 0;
}

int ui_levelmenu_draw(LevelMenu *menu){
    int ret = 0;

    menu->title.x = menu->ui->windowWidth/2 - menu->title.width/2;
    menu->title.y = menu->ui->windowHeight - TITLE_FONT_SIZE - BUTTON_GAP;

    menu->btnBack.x = BUTTON_GAP;
    menu->btnBack.y = menu->ui->windowHeight - BUTTON_GAP - BUTTON_HEIGHT*1.5f;

    float containerInitialX = menu->ui->windowWidth/2 - BUTTON_CONTAINER_WIDTH/2;
    vec2 pos = {
        containerInitialX,
        menu->ui->windowHeight - (2*TITLE_FONT_SIZE + 2*BUTTON_GAP)
    };

    ui_no_colorize(menu->ui);
    ui_image2d_draw(&menu->background);
    ui_text2d_draw(&menu->title);

    if(ui_button_draw(&menu->btnBack))
        ret = UI_LEVELMENU_BACK;

    for(uint8_t i = 0; i < menu->buttonCount; i++){
        ui_button_set_position(menu->buttons + i, pos);
        if(ui_button_draw(menu->buttons + i))
            ret = i+1;

        if(i % BUTTON_COLUMNS == BUTTON_COLUMNS - 1){
            pos[0] = containerInitialX;
            pos[1] -= BUTTON_GAP + BUTTON_HEIGHT;
        }else{
            pos[0] += BUTTON_GAP + BUTTON_WIDTH;
        }
    }
    return ret;
}

int ui_levelmenu_destroy(LevelMenu *menu){
    ui_text2d_destroy(&menu->title);
    ui_button_destroy(&menu->btnBack);
    ui_image2d_destroy(&menu->background);
    if(menu->buttonCount > 0){
        for(uint8_t i = 0; i < menu->buttonCount; i++)
            ui_button_destroy(menu->buttons + i);

        free(menu->buttons);
        menu->buttons = NULL;
    }
    return 0;
}
