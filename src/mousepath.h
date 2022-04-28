#pragma once
#include <stdlib.h>
#include <stdbool.h>
#include <SDL.h>
#include "meta/game.h"

/*
    Every Node has a next and a follow pointer.
    Next is used for the actual data List.
    Follow is used for the next node for a mouse to travel
*/
int mousepathCreate(LevelServer *ls);
