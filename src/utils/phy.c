#include "utils/phy.h"

void getDeltaTime(Uint32 *pStart, double *pDelta_time, int *pFrame_c) {
    Uint32 diff;

    if (*pFrame_c % 5 == 0 && (diff = SDL_GetTicks() - *pStart) >= 1000) {
        *pDelta_time = diff / (*pFrame_c * 1000.0);
        *pStart = SDL_GetTicks();
        *pFrame_c = 0;
    }
}

void randomGradient(int ix, int iy, int seed, float *pGradient_x, float *pGradient_y) {
    // No precomputed gradients mean this works for any number of grid coordinates
    const unsigned w = 8 * sizeof(unsigned);
    const unsigned s = w / 2;
    unsigned a = ix, b = iy;

    a *= seed;
    b ^= a << s | a >> (w - s);
    b *= seed ^ 0x9E3779B9;
    a ^= b << s | b >> (w - s);
    a *= seed * 0x85EBCA77;
    b ^= (a + seed) ^ 0xC2B2AE3D;

    float random = a * (3.14159265 / ~(~0u >> 1)); // in [0, 2*π]

    *pGradient_x = sin(random);
    *pGradient_y = cos(random);
}

float dotGridGradient(int ix, int iy, float x, float y, int seed) {
    // Get gradient from integer coordinates
    float gradient_x, gradient_y;

    randomGradient(ix, iy, seed, &gradient_x, &gradient_y);

    // Compute the distance vector
    float dx = x - ix, dy = y - iy;

    // Compute the dot-product
    return (dx * gradient_x + dy * gradient_y);
}

float perlin(float x, float y, int seed) {
    // WTF is this shit, I spent 2 days looking for the issue of why this doesn't work for
    //  negative numbers and you tell me that randomly replacing (int)x with floor(x) fixes it.
    //  FUCK YOU

    // Determine grid cell corner coordinates
    int x0 = floor(x), y0 = floor(y);
    int x1 = x0 + 1, y1 = y0 + 1;
    // Compute Interpolation weights
    float sx = x - x0, sy = y - y0;
    // Compute and interpolate top two corners
    float n0 = dotGridGradient(x0, y0, x, y, seed), n1 = dotGridGradient(x1, y0, x, y, seed);
    float ix0 = INTERPOLATE(n0, n1, sx);
    float ix1;

    // Compute and interpolate bottom two corners
    n0 = dotGridGradient(x0, y1, x, y, seed);
    n1 = dotGridGradient(x1, y1, x, y, seed);
    ix1 = INTERPOLATE(n0, n1, sx);

    // Final step: interpolate between the two previously interpolated values, now in y
    return INTERPOLATE(ix0, ix1, sy);
}

void FBM(float freq, int x, int y, int seed, uint8_t *pR, uint8_t *pG, uint8_t *pB) {
    float val = 0, amp = 1;

    for (int i = 0; i < PERLIN_OCTAVES; i++) {
        val += perlin(x * freq, y * freq, seed) * amp;
        freq *= 2;
        amp /= 2;
    }
    // Clipping.
    if (val > 1.0f)
        val = 1.0f;
    else if (val < -1.0f)
        val = -1.0f;
    //* change this to not restirct for more natural terrain.
    val = (val + 1) * 0.5; // Restricting to [0, 1]
    if (val <= 0.2) { val = 0; }
    else if (val <= 0.4) { val = 0.2; }
    else if (val <= 0.6) { val = 0.4; }
    else if (val <= 0.8) { val = 0.6; }
    else { val = 0.8; }
    *pR = *pG = *pB = val * 255;
}

void normalizeMoveDist(int vel, double delta_time, uint16_t input_flags, int *pX_norm, int *pY_norm) {
    // If only one of them is true, their respective sign will make accurate ans.
    // If both are true then they cancel out and no moving in that direction.
    int x_comp = HAS_FLAG(input_flags, KEY_RIGHT) - HAS_FLAG(input_flags, KEY_LEFT),
        y_comp = HAS_FLAG(input_flags, KEY_DOWN) - HAS_FLAG(input_flags, KEY_UP);
    float magnitude = sqrt((x_comp * x_comp) + (y_comp * y_comp));

    if (magnitude == 0.0f) {
        *pX_norm = *pY_norm = 0;
        return;
    }
    *pX_norm = (vel * x_comp * delta_time) / magnitude;
    *pY_norm = (vel * y_comp * delta_time) / magnitude;
}