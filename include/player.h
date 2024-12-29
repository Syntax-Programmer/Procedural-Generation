#ifndef PLAYER_H
#define PLAYER_H

#include "object.h"

typedef struct Player {
    Obj obj;
    Stat health;
    int vel;
} Player;

extern Player createPlayer(int x, int y, int w, int h, Uint8 r, Uint8 g, Uint8 b, Uint8 a, int can_collide,
                           Uint8 max_health, int max_vel);
extern int isPlayerDead(const Player* pPlayer);

#endif