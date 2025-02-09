#include "../include/physics.h"

/// @brief Calculates the random gradient of a given 2D point.
/// @param ix X-coordinate of the grid.
/// @param iy Y-coordinate of the grid.
/// @param seed Random seed for generating gradients.
/// @param pGradient_x Pointer to store the X component of the gradient.
/// @param pGradient_y Pointer to store the Y component of the gradient.
static void randomGradient(int32_t ix, int32_t iy, time_t seed,
                           float *pGradient_x, float *pGradient_y);

/// @brief Computes the dot product between a gradient vector and a distance vector.
/// @param ix X-coordinate of the grid.
/// @param iy Y-coordinate of the grid.
/// @param x X-coordinate of the point.
/// @param y Y-coordinate of the point.
/// @param seed Random seed for generating gradients.
/// @return The dot product result.
static float dotGridGradient(int32_t ix, int32_t iy, float x, float y, time_t seed);

///@brief Computes Perlin noise at a given point.
///@param x X-coordinate of the point.
///@param y Y-coordinate of the point.
///@param seed Random seed for generating noise.
///@return Perlin noise value in range [-1, 1].
static float perlin(float x, float y, time_t seed);

void getDeltaTime(Uint32 *pStart, double *pDelta_time, uint8_t *pFrame_c) {
    Uint32 diff;

    if (*pFrame_c % 5 == 0 && (diff = SDL_GetTicks() - *pStart) >= 1000) {
        *pDelta_time = diff / (*pFrame_c * 1000.0);
        *pStart = SDL_GetTicks();
        *pFrame_c = 0;
    }
}

static void randomGradient(int32_t ix, int32_t iy, time_t seed,
                           float *pGradient_x, float *pGradient_y) {
    // No precomputed gradients mean this works for any number of grid coordinates
    const uint32_t w = 8 * sizeof(uint32_t);
    const uint32_t s = w / 2;
    uint32_t a = ix, b = iy;

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

static float dotGridGradient(int32_t ix, int32_t iy, float x, float y, time_t seed) {
    // Get gradient from integer coordinates
    float gradient_x, gradient_y;

    randomGradient(ix, iy, seed, &gradient_x, &gradient_y);

    // Compute the distance vector
    float dx = x - ix, dy = y - iy;

    // Compute the dot-product
    return (dx * gradient_x + dy * gradient_y);
}

static float perlin(float x, float y, time_t seed) {
    // WTF is this shit, I spent 2 days looking for the issue of why this doesn't work for
    //  negative numbers and you tell me that randomly replacing (int)x with floor(x) fixes it.
    //  FUCK YOU
    // Determine grid cell corner coordinates
    int32_t x0 = floor(x), y0 = floor(y);
    int32_t x1 = x0 + 1, y1 = y0 + 1;
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

uint32_t FBM(float freq, int32_t x, int32_t y, time_t seed) {
    float val = 0, amp = 1;
    uint8_t color_val;
    uint32_t final_color;

    for (int32_t i = 0; i < OCTAVES; i++) {
        val += perlin(x * freq, y * freq, seed) * amp;
        freq *= 2;
        amp /= 2;
    }
    // Clipping.
    if (val > 1.0f)
        val = 1.0f;
    else if (val < -1.0f)
        val = -1.0f;
    //* change this to not restrict for more natural terrain.
    val = (val + 1) * 0.5; // Restricting to [0, 1]
    if (val <= 0.2) { val = 0; }
    else if (val <= 0.4) { val = 0.2; }
    else if (val <= 0.6) { val = 0.4; }
    else if (val <= 0.8) { val = 0.6; }
    else { val = 0.8; }
    color_val = (int)(val * 255);
    final_color = color_val << 24 | color_val << 16 | color_val << 8 | 255;

    return final_color;
}

Vec2 computeMoveDist(int16_t vel, double delta_time, uint16_t input_flags) {
    // If only one of them is true, their respective sign will make accurate ans.
    // If both are true then they cancel out and no moving in that direction.
    int8_t x_comp = HAS_FLAG(input_flags, RIGHT) - HAS_FLAG(input_flags, LEFT),
        y_comp = HAS_FLAG(input_flags, DOWN) - HAS_FLAG(input_flags, UP);
    float magnitude = sqrt((x_comp * x_comp) + (y_comp * y_comp));
    Vec2 norm_dist;

    if (magnitude == 0.0f) {
        norm_dist.x = norm_dist.y = 0;
    } else {
        norm_dist.x = (vel * x_comp * delta_time) / magnitude;
        norm_dist.y = (vel * y_comp * delta_time) / magnitude;
    }

    return norm_dist;
}
