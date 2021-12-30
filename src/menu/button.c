#include "button.h"

static const char* TAG = "[BUTTON]";

void uiElementsCreate(UIElements *ui, SDL_Renderer *renderer){
    char buffer[128];
    strcpy(buffer, "");
    sprintf(buffer, "%stextures/ui/background.png", ASSET_PATH);
    textureLoad(&ui->background, renderer, buffer);
    
    strcpy(buffer, "");
    sprintf(buffer, "%stextures/ui/button_long.png", ASSET_PATH);
    textureLoad(&ui->buttonsLong, renderer, buffer);
    
    strcpy(buffer, "");
    sprintf(buffer, "%stextures/ui/button_short.png", ASSET_PATH);
    textureLoad(&ui->buttonsShort, renderer, buffer);
    
    strcpy(buffer, "");
    sprintf(buffer, "%stextures/ui/button_very_short.png", ASSET_PATH);
    textureLoad(&ui->buttonsVeryShort, renderer, buffer);
}

void uiElementsDestroy(UIElements *ui){
    textureDestroy(&ui->background);
    textureDestroy(&ui->buttonsVeryShort);
    textureDestroy(&ui->buttonsShort);
    textureDestroy(&ui->buttonsLong);
}

void menuButtonCreate(Button *button, const char* title, Image *icon, UIElements *ui, uint8_t colour, ButtonType btype){
    button->ui = ui;
    button->btype = btype;
    switch(btype){
        case BT_LONG: {
            button->hitbox.w = BUTTON_WIDTH_LONG;
        }break;
        case BT_SHORT: {
            button->hitbox.w = BUTTON_WIDTH_SHORT;
        } break;
        case BT_VERY_SHORT:{
            button->hitbox.w = BUTTON_WIDTH_VERY_SHORT;
        } break;
        default: {
            SDL_LogWarn(0,"%s\tInvalid button type! Assuming long button\n", TAG);
            button->hitbox.w = BUTTON_WIDTH_LONG;
        }break;
    }
    button->hitbox.h = BUTTON_HEIGHT;
    SDL_Colour white = {
        .r = 255,
        .g = 255,
        .b = 255
    };
    
    SDL_Colour yellow = {
        .r = 255,
        .g = 255,
        .b = 64
    };
    
    fontCreate(&button->text, FONT_DEFAULT, BUTTON_TEXT_SIZE, white, ui->renderer);
    fontRender(&button->text, title);
    
    fontCreate(&button->textActive, FONT_DEFAULT, BUTTON_TEXT_SIZE, yellow, ui->renderer);
    fontRender(&button->textActive, title);
    
    switch(btype){
        case BT_LONG: {
            button->textOffset.x = (BUTTON_WIDTH_LONG - button->text.rect.w) / 2;
        }break;
        case BT_SHORT: {
            button->textOffset.x = (BUTTON_WIDTH_SHORT - button->text.rect.w) / 2;
        } break;
        case BT_VERY_SHORT:{
            //The -4 is just raw guesswork. Hence: no macro :3
            button->textOffset.x = (BUTTON_WIDTH_VERY_SHORT - button->text.rect.w) / 2 - 4;
        } break;
    }
    button->textOffset.y = (BUTTON_HEIGHT - button->text.rect.h) / 2;
    
    button->bai.period = 0;
    button->bai.frameCount = 4;
    button->bai.frameNumber = colour % button->bai.frameCount;
}

int menuButtonHandle(Button *button, int x, int y, float winscale){
    Image* btex = NULL;
    switch(button->btype){
        case BT_LONG: {
            btex = &button->ui->buttonsLong;
        }break;
        case BT_SHORT: {
            btex = &button->ui->buttonsShort;
        } break;
        case BT_VERY_SHORT:{
            btex = &button->ui->buttonsVeryShort;
        } break;
    }
    textureDraw(
        btex,
        x,
        y,
        SDL_FLIP_NONE,
        &button->bai
    );
    
    button->hitbox.x = x;
    button->hitbox.y = y;
    
    SDL_Rect acthb = {
        .x = button->hitbox.x * winscale,
        .y = button->hitbox.y * winscale,
        .w = button->hitbox.w * winscale,
        .h = button->hitbox.h * winscale,
    };
    SDL_Point mousepos;
    Uint32 qrry = SDL_GetMouseState(&mousepos.x, &mousepos.y);
    bool active = SDL_PointInRect(&mousepos, &acthb);
    
    fontDraw(
        active ? &button->textActive : &button->text,
        x + button->textOffset.x,
        y + button->textOffset.y
    );
    
    if(active){
        if(qrry & SDL_BUTTON(SDL_BUTTON_LEFT)){
            return 1;
        }
    }
    
    return 0;
}

void menuButtonDestroy(Button *button){
    fontDestroy(&button->text);
    fontDestroy(&button->textActive);
}
