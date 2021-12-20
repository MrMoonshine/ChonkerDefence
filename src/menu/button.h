#pragma once
#include <SDL.h>
#include <SDL_image.h>
#include <time.h>
#include <stdbool.h>
#include "../image.h"
#include "../text.h"

#define BUTTON_TEXT_SIZE 32
#define BUTTON_WIDTH_LONG 450
#define BUTTON_WIDTH_SHORT 240
#define BUTTON_HEIGHT 72

#define BUTTON_BLUE 0
#define BUTTON_RED 1
#define BUTTON_GREEN 2
#define BUTTON_YELLOW 3

typedef struct{
    Image buttonsLong;
    Image buttonsShort;
    Image background;
    SDL_Renderer *renderer;
} UIElements;

typedef struct{
    Font text;
    Font textActive;
    Image *icon;
    Image *background;
    AnimationInfo bai;
    UIElements *ui;
    bool isShort;
    SDL_Point textOffset;
    SDL_Rect hitbox;
}Button;

void uiElementsCreate(UIElements *ui, SDL_Renderer *renderer);
void uiElementsDestroy(UIElements *ui);

void menuButtonCreate(Button *button, const char* title, Image *icon, UIElements *ui, uint8_t colour, bool isShort);
//Returns 0. If button is pressed return 1
int menuButtonHandle(Button *button, int x, int y, float winscale);
void menuButtonDestroy(Button *button);
