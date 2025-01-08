/*
    Used a techinque called Perlin Noise with Fractal Brownian Motion here.
    For reference here are the resources I used:
    https://rtouti.github.io/graphics/perlin-noise-algorithm
    https://adrianb.io/2014/08/09/perlinnoise.html
    https://gist.github.com/Flafla2/f0260a861be0ebdeef76
*/

#include "utils/physics.h"

void getDeltaTime(time_t *pStart, double *pDelta_time, int *pFrame_c) {
    time_t diff;

    if (*pFrame_c % 5 == 0 && (diff = time(NULL) - *pStart) >= 1) {
        *pDelta_time = 1.0 / (*pFrame_c / diff);
        *pStart = time(NULL);
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
    float random = a * (3.14159265 / ~(~0u >> 1)); // in [0, 2*Pi]
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

float interpolate(float a0, float a1, float w) {
    return (a1 - a0) * ((6 * w - 15) * w + 10) * w * w * w + a0;
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
    float ix0 = interpolate(n0, n1, sx);
    float ix1;

    // Compute and interpolate bottom two corners
    n0 = dotGridGradient(x0, y1, x, y, seed);
    n1 = dotGridGradient(x1, y1, x, y, seed);
    ix1 = interpolate(n0, n1, sx);
    // Final step: interpolate between the two previously interpolated values, now in y
    return interpolate(ix0, ix1, sy);
}

void FBM(float freq, int x, int y, int seed, uint8_t *pR, uint8_t *pG, uint8_t *pB)
{
    float val = 0, amp = 1;

    for (int i = 0; i < PERLIN_OCTAVES; i++)
    {
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
    if (val <= 0.2) {
        val = 0;
    } else if (val <= 0.4) {
        val = 0.2;
    } else if (val <= 0.6) {
        val = 0.4;
    } else if (val <= 0.8) {
        val = 0.6;
    } else {
        val = 0.8;
    }
    *pR = *pG = *pB = val * 255;
}

void normalizeMoveDist(float vel, double delta_time, int x_comp, int y_comp,
                  float *pX_norm, float *pY_norm) {
    float magnitude = sqrt((x_comp * x_comp) + (y_comp * y_comp));

    if (magnitude == 0.0f)
    {
        *pX_norm = *pY_norm = 0;
        return;
    }
    *pX_norm = (vel * x_comp * delta_time) / magnitude;
    *pY_norm = (vel * y_comp * delta_time) / magnitude;
}