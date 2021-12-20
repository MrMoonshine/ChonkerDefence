#include "button.h"

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
}

void uiElementsDestroy(UIElements *ui){
    textureDestroy(&ui->background);
    textureDestroy(&ui->buttonsShort);
    textureDestroy(&ui->buttonsLong);
}

void menuButtonCreate(Button *button, const char* title, Image *icon, UIElements *ui, uint8_t colour, bool isShort){
    button->ui = ui;
    button->isShort = isShort;
    button->hitbox.w = isShort ? BUTTON_WIDTH_SHORT : BUTTON_WIDTH_LONG;
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
    button->textOffset.x = ((isShort ? BUTTON_WIDTH_SHORT : BUTTON_WIDTH_LONG) - button->text.rect.w) / 2;
    button->textOffset.y = (BUTTON_HEIGHT - button->text.rect.h) / 2;
    
    //printf("Text Offset is: %d | %d\n", button->textOffset.x, button->textOffset.y);
    
    button->bai.period = 0;
    button->bai.frameCount = 4;
    button->bai.frameNumber = colour % button->bai.frameCount;
}

int menuButtonHandle(Button *button, int x, int y, float winscale){
    textureDraw(
        button->isShort ? &button->ui->buttonsShort : &button->ui->buttonsLong,
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
