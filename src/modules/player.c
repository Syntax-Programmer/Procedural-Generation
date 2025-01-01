#include "player.h"

Player createPlayer(int x, int y, int w, int h, Uint32 color_hex, int can_collide,
                           Uint8 max_health, int max_vel) {
    Player player;

    player.obj = createObj(x, y, w, h, color_hex, can_collide);
    player.health = createStat(max_health);
    player.vel = max_vel;
    return player;
}

int isPlayerDead(const Player* pPlayer) {
    if (pPlayer->health.curr == 0) {
        return 1;
    }
    return 0;
}