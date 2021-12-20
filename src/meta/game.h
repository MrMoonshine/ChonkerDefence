#pragma once
#define GAME_PLAYERS_MAX 3
#define GAME_PLAYER_NAME_LENGTH 10


typedef struct{
    char name[GAME_PLAYER_NAME_LENGTH];
    uint8_t profileImage;
}PlayerInfo;
