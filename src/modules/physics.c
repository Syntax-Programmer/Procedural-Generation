#include "physics.h"

void getDeltaTime(time_t *pStart, double *pDelta_time, int *pFrame_c)
{
    time_t diff;

    if (*pFrame_c % 5 == 0 && (diff = time(NULL) - *pStart) >= 1)
    {
        *pDelta_time = 1.0 / (*pFrame_c / diff);
        *pStart = time(NULL);
        *pFrame_c = 0;
    }
}

SDL_Rect createRect(int x, int y, int w, int h)
{
    return (SDL_Rect){x, y, w, h};
}

void setRectPos(SDL_Rect *pRect, int x, int y)
{
    pRect->x = x;
    pRect->y = y;
}

void setRectSize(SDL_Rect *pRect, int w, int h)
{
    pRect->w = w;
    pRect->h = h;
}

void getWorldMovOffset(int vel, double delta_time, int x_comp, int y_comp, int *pX_norm, int *pY_norm)
{
    float magnitude = sqrt((x_comp * x_comp) + (y_comp * y_comp));
    if (magnitude == 0.0f)
    {
        return;
    }
    float norm_x = x_comp / magnitude;
    float norm_y = y_comp / magnitude;
    int delta_dist = vel * delta_time;

    *pX_norm = (int)(norm_x * delta_dist);
    *pY_norm = (int)(norm_y * delta_dist);
}

int AABBCollision(const SDL_Rect *pRect1, const SDL_Rect *pRect2)
{
    return (
        pRect1->x < pRect2->x + pRect2->w &&
        pRect1->x + pRect1->w > pRect2->x &&
        pRect1->y < pRect2->y + pRect2->h &&
        pRect1->y + pRect1->h > pRect2->y);
}

int isRectInFOV(const SDL_Rect *pRect)
{
    if (pRect->x > SCREEN_WIDTH || pRect->x + pRect->w < 0 ||
        pRect->y > SCREEN_HEIGHT || pRect->y + pRect->h < 0)
    {
        return 0;
    }
    return 1;
}

vector2 randomGradient(int ix, int iy, int seed)
{
    // No precomputed gradients mean this works for any number of grid coordinates
    const unsigned w = 8 * sizeof(unsigned);
    const unsigned s = w / 2;
    unsigned a = ix, b = iy;
    vector2 v;

    a *= seed;
    b ^= a << s | a >> (w - s);
    b *= seed ^ 0x9E3779B9;
    a ^= b << s | b >> (w - s);
    a *= seed * 0x85EBCA77;
    b ^= (a + seed) ^ 0xC2B2AE3D;
    float random = a * (3.14159265 / ~(~0u >> 1)); // in [0, 2*Pi]
    // Create the vector from the angle
    v.x = sin(random);
    v.y = cos(random);

    return v;
}

float dotGridGradient(int ix, int iy, float x, float y, int seed)
{
    // Get gradient from integer coordinates
    vector2 gradient = randomGradient(ix, iy, seed);
    // Compute the distance vector
    float dx = x - ix, dy = y - iy;

    // Compute the dot-product
    return (dx * gradient.x + dy * gradient.y);
}

float interpolate(float a0, float a1, float w)
{
    return (a1 - a0) * ((6 * w - 15) * w + 10) * w * w * w + a0;
}