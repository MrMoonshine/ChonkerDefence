#include "image.h"

static const char* ERROR_MSG_DRAW = "Failed to draw Image!\t%s\n";

int textureLoad(Image *image, SDL_Renderer* renderer, const char* filename){
    image->valid = false;
    //Assign Renderer
    image->renderer = renderer;
    image->destination.x = 0;
    image->destination.y = 0;
    if(!filename)
        return 1;
    //Load Texture
    image->texture = IMG_LoadTexture(image->renderer,filename);
    if(!image->texture){
        fprintf(stderr, "Failed to load: %s\tError: %s\n", filename, IMG_GetError());
        return -1;
    }

    SDL_QueryTexture(
        image->texture,
        NULL,
        NULL,
        &image->destination.w,
        &image->destination.h
    );
    
    image->valid = true;
    return 0;
}

int textureDestroy(Image *image){
    if(image->valid)
    SDL_DestroyTexture(image->texture);
    return 0;
}

void textureDraw(Image *image, int x, int y, SDL_RendererFlip flip, AnimationInfo *animation){
    textureDrawRotated(image, x, y, flip, animation, 0.0);
}

void textureDrawRotated(Image *image, int x, int y, SDL_RendererFlip flip, AnimationInfo *animation, const double angle){
    image->destination.x = x;
    image->destination.y = y;
    if(animation){
        SDL_Rect selectedArea = {
            .w = image->destination.w,
            .h = image->destination.h/animation->frameCount,
            .x = 0,
            .y = 0
        };
        
        selectedArea.y = selectedArea.h * (animation->frameNumber % animation->frameCount);
        if(animation->period > 0 && (SDL_GetTicks() - animation->lastSample > animation->period)){
            //printf("Frame Number: %d\n", selectedArea.y);
            //printf("Frame: %d x %d\n", selectedArea.w, selectedArea.h);
            animation->frameNumber++;
            animation->lastSample = SDL_GetTicks();
        }
        
        SDL_Point point1 = {
            .x = 0,
            .y = 0
        };
        
        SDL_Rect tmp = image->destination;
        tmp.h /=  animation->frameCount;
        
        if(SDL_RenderCopyEx(
            image->renderer,
            image->texture,         //Texture
            &selectedArea,              //Source Rectangle
            &tmp,    //Destination
            angle,                          //Angle
            &point1,                    //point
            flip                        //Flip
        )){
            fprintf(stderr, ERROR_MSG_DRAW, SDL_GetError());
        }
        return;
    }
    
    if(SDL_RenderCopyEx(
        image->renderer,
        image->texture,            //Texture
        NULL,               //Source Rectangle
        &image->destination,//Destination
        angle,                  //Angle
        NULL,               //point
        flip                //Flip
    )){
        fprintf(stderr, ERROR_MSG_DRAW, SDL_GetError());
    }
}

SDL_Point textureGetSize(Image *image){
    SDL_Point size;
    SDL_QueryTexture(image->texture, NULL, NULL, &size.x, &size.y);
    return size;
}
