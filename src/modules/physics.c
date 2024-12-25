#include "physics.h"

void getDeltaTime(time_t* pStart, double* pDelta_time, int* pFrame_c) {
    time_t diff;

    if (*pFrame_c % 5 == 0 && (diff = time(NULL) - *pStart) >= 1) {
        *pDelta_time = 1.0 / (*pFrame_c / diff);
        *pStart = time(NULL);
        *pFrame_c = 0;
    }
}

SDL_Rect createRect(int x, int y, int w, int h) {
    return (SDL_Rect){x, y, w, h};
}

void setRectPos(SDL_Rect* pRect, int x, int y) {
    pRect->x = x;
    pRect->y = y;
}

void setRectSize(SDL_Rect* pRect, int w, int h) {
    pRect->w = w;
    pRect->h = h;
}

void getWorldMovOffset(int vel, double delta_time, int x_comp, int y_comp, int* pX_norm, int* pY_norm) {
    float magnitude = sqrt((x_comp * x_comp) + (y_comp * y_comp));
    if (magnitude == 0.0f) {
        return;
    }
    float norm_x = x_comp / magnitude;
    float norm_y = y_comp / magnitude;
    int delta_dist = vel * delta_time;

    *pX_norm = (int)(norm_x * delta_dist);
    *pY_norm = (int)(norm_y * delta_dist);
}

void moveRect(SDL_Rect* pRect, int x_offset, int y_offset) {
    pRect->x -= x_offset;
    pRect->y -= y_offset;
}

int AABBCollision(const SDL_Rect* pRect1, const SDL_Rect* pRect2) {
    return (
        pRect1->x < pRect2->x + pRect2->w &&
        pRect1->x + pRect1->w > pRect2->x &&
        pRect1->y < pRect2->y + pRect2->h &&
        pRect1->y + pRect1->h > pRect2->y
    );
}