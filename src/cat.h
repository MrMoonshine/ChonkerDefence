#pragma once
#include <SDL.h>
#include <SDL_image.h>
#include <time.h>
#include "nodes.h"
#include "image.h"

#define CAT_NAME_LENGTH 16
#define CAT_NAME_FILE "cats.names"
//Further information can be found at Joplin
#define CAT_FRONT_MOUTH_TEXPATH "%scat_mouth_front_animated.png"
#define CAT_FRONT_HEAD_TEXPATH_PATTERN "%scat_%s_front_head.png"
#define CAT_FRONT_BODY_TEXPATH_PATTERN "%scat_%s_front_body_%d.png"

#define CAT_REAR_HEAD_TEXPATH_PATTERN "%scat_%s_rear_head.png"
#define CAT_REAR_BODY_TEXPATH_PATTERN "%scat_%s_rear_body_%d.png"
#define CAT_REAR_TAIL_TEXPATH_PATTERN "%scat_%s_rear_tail.png"

#define CAT_SIDE_MOUTH_TEXPATH "%scat_mouth_side_animated.png"
#define CAT_SIDE_HEAD_TEXPATH_PATTERN "%scat_%s_side_head.png"
#define CAT_SIDE_BODY_TEXPATH_PATTERN "%scat_%s_side_body_%d.png"
#define CAT_SIDE_TAIL_TEXPATH_PATTERN "%scat_%s_side_tail.png"

#define CAT_CHONKYNESS_COUNT 6
typedef enum {
    A_FINE_BOI = 0,
    HE_CHOMNK = 1,
    A_HECKIN_CHONKER = 2,
    HEFTY_CHONK = 3,
    MEGA_CHONKER = 4,
    OH_LAWD_HE_COMIN = 5
} Chonkyness;

typedef struct {
    /*-------------------------------*/
    /*    Front View Textures        */
    /*-------------------------------*/
    Image frontHead;                //Head at Front View
    Image frontBody[CAT_CHONKYNESS_COUNT];                //Body at front view
    Image frontMouth;               //Mouth for Front view
    /*-------------------------------*/
    /*    Side  View Textures        */
    /*-------------------------------*/
    Image sideHead;                //Head at side View
    Image sideBody[CAT_CHONKYNESS_COUNT];                //Body at side view
    Image sideMouth;               //Mouth for side view
    Image sideTail;                //Tail for side view
    /*-------------------------------*/
    /*    Rear View Textures         */
    /*-------------------------------*/
    Image rearHead;                 //Head for Rear view
    Image rearBody[CAT_CHONKYNESS_COUNT];                 //Head for Rear view
    Image rearTail;                 //Tail for Rear and Front view
} CTS;  // Cat Texture Set

typedef struct {
    char name[CAT_NAME_LENGTH];     //Cat's Name
    uint8_t fur;                    //Fur Colour
    Nodepos position;               //Cat's Position
    Rotation rotation;              //Cat's orientation
    Chonkyness chonkyness;
    CTS* textures;
} Cat;


void ctsCreate(CTS *cts, SDL_Renderer* renderer, const char* fur);
void ctsDestroy(CTS *cts);

void catCreate(Cat *cat, CTS *textures, Chonkyness chonkyness);
void catDestroy(Cat *cat);
void catDraw(Cat *cat);
