#include "cat.h"

static AnimationInfo CAT_ANIMATION_MOUTH = {
    .period = 1000,
    .frameCount = 3,
    .frameNumber = 0
};

void ctsCreate(CTS *cts, SDL_Renderer* renderer, const char* fur){
    //Texture Root path
    char texturepath[128];
    strcpy(texturepath, ASSET_PATH);
    strcat(texturepath, "textures/entity/");
    char filename[128] = "";
    /*-------------------------------*/
    /*    Front View Textures        */
    /*-------------------------------*/   
    sprintf(filename, CAT_FRONT_MOUTH_TEXPATH, texturepath);
    textureLoad(&cts->frontMouth, renderer, filename);
    sprintf(filename, CAT_FRONT_HEAD_TEXPATH_PATTERN, texturepath, fur);
    textureLoad(&cts->frontHead, renderer, filename);
    for(int a = 0; a < CAT_CHONKYNESS_COUNT; a++){
        sprintf(filename, CAT_FRONT_BODY_TEXPATH_PATTERN, texturepath, fur, a);
        textureLoad(&cts->frontBody[a], renderer, filename);
    }
    /*-------------------------------*/
    /*    Side View Textures         */
    /*-------------------------------*/
    sprintf(filename, CAT_SIDE_HEAD_TEXPATH_PATTERN, texturepath, fur);
    textureLoad(&cts->sideHead, renderer, filename);
    sprintf(filename, CAT_SIDE_TAIL_TEXPATH_PATTERN, texturepath, fur);
    textureLoad(&cts->sideTail, renderer, filename);
    for(int a = 0; a < CAT_CHONKYNESS_COUNT; a++){
        sprintf(filename, CAT_SIDE_BODY_TEXPATH_PATTERN, texturepath, fur, a);
        textureLoad(&cts->sideBody[a], renderer, filename);
    }
    /*-------------------------------*/
    /*    Rear View Textures         */
    /*-------------------------------*/
    sprintf(filename, CAT_REAR_TAIL_TEXPATH_PATTERN, texturepath, fur);
    textureLoad(&cts->rearTail, renderer, filename);
    sprintf(filename, CAT_REAR_HEAD_TEXPATH_PATTERN, texturepath, fur);
    textureLoad(&cts->rearHead, renderer, filename);
    for(int a = 0; a < CAT_CHONKYNESS_COUNT; a++){
        sprintf(filename, CAT_REAR_BODY_TEXPATH_PATTERN, texturepath, fur, a);
        textureLoad(&cts->rearBody[a], renderer, filename);
    }
}

void ctsDestroy(CTS* cts){
    textureDestroy(&cts->frontMouth);
    textureDestroy(&cts->frontHead);
    
    textureDestroy(&cts->sideMouth);
    textureDestroy(&cts->sideHead);
    textureDestroy(&cts->sideTail);  
    
    textureDestroy(&cts->rearTail);
    textureDestroy(&cts->rearHead);
    for(int a = 0; a < CAT_CHONKYNESS_COUNT; a++){
        textureDestroy(&cts->rearBody[a]);  
        textureDestroy(&cts->sideBody[a]);
        textureDestroy(&cts->frontBody[a]);
    }
}

void catCreate(Cat *cat, CTS *textures, Chonkyness chonkyness){
    cat->textures = textures;
    cat->chonkyness = chonkyness;
    cat->position.x = 320;
    cat->position.y = 80;
    cat->rotation = FRONT;

}
void catDestroy(Cat *cat){

    
}

void catDraw(Cat *cat){
    switch (cat->rotation){
        case LEFT: {
            textureDraw(
                &cat->textures->sideTail,
                cat->position.x + 56,
                cat->position.y-16,
                SDL_FLIP_HORIZONTAL,
                NULL
            );
            textureDraw(
                &cat->textures->sideBody[cat->chonkyness],
                cat->position.x,
                cat->position.y,
                SDL_FLIP_NONE,
                NULL
            );
            textureDraw(
                &cat->textures->sideHead,
                cat->position.x,
                cat->position.y,
                SDL_FLIP_NONE,
                &CAT_ANIMATION_MOUTH
            );
        } break;
        case RIGHT: {
            textureDraw(
                &cat->textures->sideTail,
                cat->position.x - 28,
                cat->position.y-16,
                SDL_FLIP_NONE,
                NULL
            );
            textureDraw(
                &cat->textures->sideBody[cat->chonkyness],
                cat->position.x,
                cat->position.y,
                SDL_FLIP_HORIZONTAL,
                NULL
            );
            textureDraw(
                &cat->textures->sideHead,
                cat->position.x + 64,
                cat->position.y,
                SDL_FLIP_HORIZONTAL,
                &CAT_ANIMATION_MOUTH
            );
        } break;
        case REAR: {
            textureDraw(
                &cat->textures->rearHead,
                cat->position.x + 16,
                cat->position.y,
                SDL_FLIP_NONE,
                NULL
            );
            textureDraw(
                &cat->textures->rearBody[cat->chonkyness],
                cat->position.x,
                cat->position.y,
                SDL_FLIP_NONE,
                NULL
            );
            textureDraw(
                &cat->textures->rearTail,
                cat->position.x,
                cat->position.y-16,
                SDL_FLIP_NONE,
                NULL
            );
        }break;
        case FRONT: {
            textureDraw(
                &cat->textures->rearTail,
                cat->position.x,
                cat->position.y-16,
                SDL_FLIP_NONE,
                NULL
            );
            textureDraw(
                &cat->textures->frontBody[cat->chonkyness],
                cat->position.x,
                cat->position.y,
                SDL_FLIP_NONE,
                NULL
            );
            textureDraw(
                &cat->textures->frontHead,
                cat->position.x + 16,
                cat->position.y,
                SDL_FLIP_NONE,
                NULL
            );
            textureDraw(
                &cat->textures->frontMouth,
                cat->position.x + 16,
                cat->position.y,
                SDL_FLIP_NONE,
                &CAT_ANIMATION_MOUTH
            );
        }break;
    }
}
