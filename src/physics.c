#include "../include/physics.h"

/*
 * @brief Calculates the random gradient of the given 2-D point.
 *
 * Using bit manipulation we calculate the random gradient of the fiven grid pos.
 * Using no precomputed gradients means this works for any number of grid coordinates.
 *
 * @param pGrid_pos The int position vector of the grid to calculate the gradient for.
 * @param seed The seed for the current session of this code running used to
 *             update/handle things that were generated with or require randomness.
 * @param pGradient The variable that will store the given gradient of the given point.
 */
static void randomGradient(IntVec2 *pGrid_pos, time_t seed,
                           FloatVec2 *pGradient);

/*
 * @brief Computes the dot product between the given the two given position vectors.
 *
 * Computes the dot product b/w the grid and point pos with a random gradient computed.
 *
 * @param pGrid_pos The int position vector of the grid to calculate the dot product for.
 * @param pPoint_pos The position vector of the random point in the grid.
 * @param seed The seed for the current session of this code running used to
 *             update/handle things that were generated with or require randomness.
 *
 * @return The computed dot product with randomness.
 */
static float dotGridGradient(IntVec2 *pGrid_pos, FloatVec2 *pPoint_pos, time_t seed);

/*
 * @brief Computes the perlin noise output for the given point in a grid.
 *
 * Using the given pos of the point we calculate the 4 surrounding points
 * and interpolate the dot product for each of them to get a final product.
 *
 * @param pPoint_pos The position vector of the random point in the grid.
 * @param seed The seed for the current session of this code running used to
 *             update/handle things that were generated with or require randomness.
 *
 * @return The perlin noise value in the range -1 to 1.
 */
static float perlin(FloatVec2 *pPoint_pos, time_t seed);

void getDeltaTime(Uint32 *pStart, double *pDelta_time, uint8_t *pFrame_c) {
    Uint32 diff;

    if (*pFrame_c % 5 == 0 && (diff = SDL_GetTicks() - *pStart) >= 1000) {
        *pDelta_time = diff / (*pFrame_c * 1000.0);
        *pStart = SDL_GetTicks();
        *pFrame_c = 0;
    }
}

static void randomGradient(IntVec2 *pGrid_pos, time_t seed,
                           FloatVec2 *pGradient) {
    const uint32_t w = 8 * sizeof(uint32_t);
    const uint32_t s = w / 2;
    uint32_t a = pGrid_pos->x, b = pGrid_pos->y;

    a *= seed;
    b ^= a << s | a >> (w - s);
    b *= seed ^ 0x9E3779B9;
    a ^= b << s | b >> (w - s);
    a *= seed * 0x85EBCA77;
    b ^= (a + seed) ^ 0xC2B2AE3D;

    float random = a * (3.14159265 / ~(~0u >> 1)); // in [0, 2*π]

    pGradient->x = sin(random);
    pGradient->y = cos(random);
}

static float dotGridGradient(IntVec2 *pGrid_pos, FloatVec2 *pPoint_pos, time_t seed) {
    // Get gradient from integer coordinates
    FloatVec2 gradient;

    randomGradient(pGrid_pos, seed, &gradient);

    // Compute the distance vector
    float dx = pPoint_pos->x - pGrid_pos->x, dy = pPoint_pos->y - pGrid_pos->y;

    // Compute the dot-product
    return (dx * gradient.x + dy * gradient.y);
}

static float perlin(FloatVec2 *pPoint_pos, time_t seed) {
    // WTF is this shit, I spent 2 days looking for the issue of why this doesn't work for
    // negative numbers and you tell me that randomly replacing (int)x with floor(x) fixes it.
    // FUCK YOU
    // Determine grid cell corner coordinates
    IntVec2 bl_grid_pos = {.x = floor(pPoint_pos->x), .y = floor(pPoint_pos->y)};
    IntVec2 br_grid_pos = {.x = bl_grid_pos.x + 1, .y = bl_grid_pos.y};
    IntVec2 tl_grid_pos = {.x = bl_grid_pos.x, .y = bl_grid_pos.y + 1};
    IntVec2 tr_grid_pos = {.x = bl_grid_pos.x + 1, .y = bl_grid_pos.y + 1};
    // Compute Interpolation weights
    float interp_weight_x = pPoint_pos->x - bl_grid_pos.x, interp_weight_y = pPoint_pos->y - bl_grid_pos.y;
    // Compute and interpolate top two corners
    float bl_gradient = dotGridGradient(&bl_grid_pos, pPoint_pos, seed),
          br_gradient = dotGridGradient(&br_grid_pos, pPoint_pos, seed),
          tl_gradient = dotGridGradient(&tl_grid_pos, pPoint_pos, seed),
          tr_gradient = dotGridGradient(&tr_grid_pos, pPoint_pos, seed);
    float bottom_interp = INTERPOLATE(bl_gradient, br_gradient, interp_weight_x),
          top_interp = INTERPOLATE(tl_gradient, tr_gradient, interp_weight_x);

    // Final step: interpolate between the two previously interpolated values, now in y
    return INTERPOLATE(bottom_interp, top_interp, interp_weight_y);
}

uint32_t FBM(float freq, IntVec2 *pGrid_pos, time_t seed) {
    float val = 0, amp = 1;
    uint8_t color_val;
    uint32_t final_color;
    FloatVec2 point;

    for (int32_t i = 0; i < OCTAVES; i++) {
        point.x = pGrid_pos->x * freq;
        point.y = pGrid_pos->y * freq;
        val += perlin(&point, seed) * amp;
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

IntVec2 computeMoveDist(int16_t vel, double delta_time, uint16_t input_flags) {
    // If only one of them is true, their respective sign will make accurate ans.
    // If both are true then they cancel out and no moving in that direction.
    int8_t x_comp = HAS_FLAG(input_flags, RIGHT) - HAS_FLAG(input_flags, LEFT),
        y_comp = HAS_FLAG(input_flags, DOWN) - HAS_FLAG(input_flags, UP);
    float magnitude = sqrt((x_comp * x_comp) + (y_comp * y_comp));
    IntVec2 norm_dist;

    if (magnitude == 0.0f) {
        norm_dist.x = norm_dist.y = 0;
    } else {
        norm_dist.x = (vel * x_comp * delta_time) / magnitude;
        norm_dist.y = (vel * y_comp * delta_time) / magnitude;
    }

    return norm_dist;
}
