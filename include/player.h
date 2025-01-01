#ifndef PLAYER_H
#define PLAYER_H

#include "object.h"

typedef struct Player {
    Obj obj;
    Stat health;
    int vel;
} Player;

extern Player createPlayer(int x, int y, int w, int h, Uint32 color_hex, int can_collide,
                           Uint8 max_health, int max_vel) ;
extern int isPlayerDead(const Player* pPlayer);

#endif