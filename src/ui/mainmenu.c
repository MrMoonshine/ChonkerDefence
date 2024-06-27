#include <ui/mainmenu.h>

int ui_mainmenu_create(MainMenu* menu, UI* ui){
    menu->ui = ui;
    //Background
    int bgwidth = APP_WIDTH * 1.3 + 32, bgheight = APP_HEIGHT*1.2 + 32;
    ui_container_create_from_png(&menu->background, ui, &bgwidth, &bgheight, -16, 0.0f, "../build/test4.png");

    //Buttons
    ui_button_create(&menu->btnSingleplayer, menu->ui, 512, 64, "Singleplayer");
    menu->btnSingleplayer.x = 256;
    menu->btnSingleplayer.y = 256;

    return 0;
}

void ui_mainmenu_draw(MainMenu* menu){
    ui_container_draw(&menu->background);
    ui_button_draw(&menu->btnSingleplayer);
}

void ui_mainmenu_destroy(MainMenu* menu){
    ui_button_destroy(&menu->btnSingleplayer);
    ui_container_destroy(&menu->background);
}
