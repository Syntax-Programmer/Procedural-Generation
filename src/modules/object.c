#include "object.h"

Stat createStat(Uint8 max)
{
    return (Stat){max, max};
}

void setStatMax(Stat *pStat, Uint8 max)
{
    if (max == 0)
    {
        fprintf(stderr, "WARNING: Tried to set max value of stat bar to 0.\n");
        return;
    }
    pStat->max = max;
    if (max < pStat->curr)
    {
        pStat->curr = max;
    }
}

void setStatCurr(Stat *pStat, int change)
{
    int new_curr = pStat->curr + change;

    if (new_curr < 0)
    {
        new_curr = 0;
    }
    else if (new_curr > pStat->max)
    {
        new_curr = pStat->max;
    }
    pStat->curr = new_curr;
}

Uint32 encodeColor(Uint8 r, Uint8 g, Uint8 b, Uint8 a)
{
    return (r << 24) | (g << 16) | (b << 8) | a;
}

void decodeColor(Uint32 color_hex, Uint8 *pR, Uint8 *pG, Uint8 *pB, Uint8 *pA)
{
    *pR = (color_hex >> 24) & 0xFF;
    *pG = (color_hex >> 16) & 0xFF;
    *pB = (color_hex >> 8) & 0xFF;
    *pA = color_hex & 0xFF;
}

Obj createObj(int x, int y, int w, int h, Uint32 color_hex, int can_collide)
{
    Obj obj;

    obj.rect = createRect(x, y, w, h);
    obj.color_hex = color_hex;
    obj.can_collide = can_collide;
    return obj;
}