#include "object.h"

Stat createStat(Uint8 max) {
    return (Stat){max, max};
}

void setStatMax(Stat* pStat, Uint8 max) {
    if (max == 0) {
        fprintf(stderr, "WARNING: Tried to set max value of stat bar to 0.\n");
        return;
    }
    pStat->max = max;
    if (max < pStat->curr) {
        pStat->curr = max;
    }
}

void setStatCurr(Stat* pStat, int change) {
    int new_curr = pStat->curr + change;

    if (new_curr < 0) {
        new_curr = 0;
    } else if (new_curr > pStat->max) {
        new_curr = pStat->max;
    }
    pStat->curr = new_curr;
}

Obj createObj(int x, int y, int w, int h, Uint8 r, Uint8 g, Uint8 b, Uint8 a) {
    Obj obj;

    obj.rect = createRect(x, y, w, h);
    obj.r = r;
    obj.g = g;
    obj.b = b;
    obj.a = a;
    return obj;
}