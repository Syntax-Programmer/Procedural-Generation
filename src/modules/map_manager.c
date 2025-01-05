/*
    Used a techinque called Perlin Noise with Fractal Brownian Motion to generate the map.
    For reference here is the resource I used:
    https://rtouti.github.io/graphics/perlin-noise-algorithm
    https://adrianb.io/2014/08/09/perlinnoise.html
    https://gist.github.com/Flafla2/f0260a861be0ebdeef76
*/

#include "map_manager.h"

static float perlin(float x, float y, int seed);
static Uint32 FBM(float freq, int x, int y, int seed);
static void freeColData(ScreenColData *to_free);

static float perlin(float x, float y, int seed)
{
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

static Uint32 FBM(float freq, int x, int y, int seed)
{
    float val = 0, amp = 1;

    for (int i = 0; i < PERLIN_OCTAVES; i++)
    {
        val += perlin(x * freq, y * freq, seed) * amp;
        freq *= 2;
        amp /= 2;
    }
    val *= 1.2; // Constrast
    // Clipping.
    if (val > 1.0f)
        val = 1.0f;
    else if (val < -1.0f)
        val = -1.0f;
    //* change this to not restirct for more natural terrain.
    val = (val + 1) * 0.5; // Restricting to [0, 1]
    if (val <= 0.2)
    {
        return encodeColor(123, 232, 33, 255);
    }
    else if (val <= 0.4)
    {
        return encodeColor(118, 182, 196, 255);
    }
    else if (val <= 0.6)
    {
        return encodeColor(194, 178, 128, 255);
    }
    else if (val <= 0.8)
    {
        return encodeColor(124, 252, 0, 255);
    }
    else
    {
        return encodeColor(250, 250, 250, 255);
    }
}

static void freeColData(ScreenColData *to_free)
{
    if (!to_free)
        return;

    ScreenColData *current = to_free;
    ScreenColData *next;

    do
    {
        next = current->next;
        free(current);
        current = next;
    } while (current != to_free);
}

int addColNode(ScreenColData **to_add, int add_to_start,
               int x, int y, int w, int h, int can_collide, Uint32 color_hex)
{
    Obj obj = createObj(x, y, w, h, color_hex, can_collide);
    ScreenColData *col_node = malloc(sizeof(ScreenColData));

    if (!col_node)
    {
        fprintf(stderr, "Failed to allocate memory for ScreenColData node.\n");
        freeColData(*to_add);
        *to_add = NULL;
        return 0;
    }
    col_node->obj = obj;
    if (!*to_add)
    {
        col_node->next = col_node;
        col_node->prev = col_node;
    }
    else
    {
        col_node->next = *to_add;
        col_node->prev = (*to_add)->prev;
        (*to_add)->prev->next = col_node;
        (*to_add)->prev = col_node;
    }
    if (!*to_add || add_to_start)
    {
        (*to_add) = col_node;
    }
    return 1;
}

void freeScreenData(ScreenColData **to_free)
{
    for (int i = 0; i < ROW_COUNT && to_free[i]; i++)
    {
        freeColData(to_free[i]);
    }
    free(to_free);
}

ScreenColData **initScreenData(float freq, int seed)
{
    /*
    Using array of circular doubly linked lists instead of array grid, so that
    when moving horizontally, we don't have to go through the entire col array,
    shifting the items just to move 1 tile over.
    */
    ScreenColData **screen_data = malloc(ROW_COUNT * sizeof(ScreenColData *)); // NOTE: This ScreenColData** is not equivalent to the screeColData** in addColNode()

    if (!screen_data)
    {
        fprintf(stderr, "Failed to allocate memory for screen data\n");
        return NULL;
    }
    for (int i = 0; i < ROW_COUNT; i++)
    {
        screen_data[i] = NULL;
        for (int j = 0; j < COL_COUNT; j++)
        {
            if (!addColNode(&screen_data[i], 0, j * TILE_SQUARE_SIDE, i * TILE_SQUARE_SIDE,
                            TILE_SQUARE_SIDE, TILE_SQUARE_SIDE, 0, FBM(freq, j, i, seed)))
            {
                freeScreenData(screen_data);
                return NULL;
            }
        }
    }
    return screen_data;
}

void findDataOutOfFOV(ScreenColData **data, int accumulated_x_offset, int accumulated_y_offset,
                      int *pX_out_of_fov, int *pY_out_of_fov)
{
    /*
    IMPORTANT: This function does not currently acount for if the player vel > TILE_SQUARE_SIDE in a single frame.
    */
    SDL_Rect tr = data[0]->prev->obj.rect, tl = data[0]->obj.rect,
             br = data[ROW_COUNT - 1]->prev->obj.rect, bl = data[ROW_COUNT - 1]->obj.rect;

    // This brings the rect down to, as they will be visible to the player.
    setRectPos(&tr, tr.x - accumulated_x_offset, tr.y - accumulated_y_offset);
    setRectPos(&tl, tl.x - accumulated_x_offset, tl.y - accumulated_y_offset);
    setRectPos(&br, br.x - accumulated_x_offset, br.y - accumulated_y_offset);
    setRectPos(&bl, bl.x - accumulated_x_offset, bl.y - accumulated_y_offset);

    int tr_in_fov = isRectInFOV(&tr), tl_in_fov = isRectInFOV(&tl),
        br_in_fov = isRectInFOV(&br), bl_in_fov = isRectInFOV(&bl);

    *pX_out_of_fov = *pY_out_of_fov = 0;
    /* NOTE: This particular order of if-elseif should be maintained and not changed
    This is because the above ones have more constraints than the lower ones, so we need
    to check for them first.*/
    // 0 means in fov, 1 means out of fov from +ve side, -1 means out of fov from -ve side.
    if (!tr_in_fov && !tl_in_fov && !br_in_fov)
    {
        // Moving bottom left
        *pX_out_of_fov = 1;
        *pY_out_of_fov = -1;
    }
    else if (!bl_in_fov && !tl_in_fov && !br_in_fov)
    {
        // Moving up right
        *pX_out_of_fov = -1;
        *pY_out_of_fov = 1;
    }
    else if (!tl_in_fov)
    {
        // They both combine to check for downward right movement.
        if (!tr_in_fov)
        {
            *pY_out_of_fov = -1; // Moving down.
        }
        if (!bl_in_fov)
        {
            *pX_out_of_fov = -1; // Moving right.
        }
    }
    else if (!br_in_fov)
    {
        // They both combine to check for upward left movement.
        if (!bl_in_fov)
        {
            *pY_out_of_fov = 1; // Moving up.
        }
        if (!tr_in_fov)
        {
            *pX_out_of_fov = 1; // Moving left.
        }
    }
}

int updateDataOutOfFOV(ScreenColData **data, int x_out_of_fov, int y_out_of_fov, float freq, int seed)
{
    ScreenColData *curr;

    if (y_out_of_fov == -1)
    {
        freeColData(data[0]);
        for (int i = 0; i < ROW_COUNT - 1; i++)
        {
            data[i] = data[i + 1];
        }
        data[ROW_COUNT - 1] = NULL;
        curr = data[ROW_COUNT - 2];
        for (int i = 0; i < COL_COUNT; i++)
        {
            if (!addColNode(&data[ROW_COUNT - 1], 0, curr->obj.rect.x, curr->obj.rect.y + TILE_SQUARE_SIDE,
                            TILE_SQUARE_SIDE, TILE_SQUARE_SIDE, 0, FBM(freq, curr->obj.rect.x / TILE_SQUARE_SIDE, (curr->obj.rect.y + TILE_SQUARE_SIDE) / TILE_SQUARE_SIDE, seed)))
            {
                freeScreenData(data);
                return 0;
            }
            curr = curr->next;
        }
    }
    else if (y_out_of_fov == 1)
    {
        freeColData(data[ROW_COUNT - 1]);
        for (int i = ROW_COUNT - 2; i >= 0; i--)
        {
            data[i + 1] = data[i];
        }
        data[0] = NULL;
        curr = data[1];
        for (int i = 0; i < COL_COUNT; i++)
        {
            if (!addColNode(&data[0], 0, curr->obj.rect.x, curr->obj.rect.y - TILE_SQUARE_SIDE,
                            TILE_SQUARE_SIDE, TILE_SQUARE_SIDE, 0, FBM(freq, curr->obj.rect.x / TILE_SQUARE_SIDE, (curr->obj.rect.y - TILE_SQUARE_SIDE) / TILE_SQUARE_SIDE, seed)))
            {
                freeScreenData(data);
                return 0;
            }
            curr = curr->next;
        }
    }
    if (x_out_of_fov == -1)
    {
        for (int i = 0; i < ROW_COUNT; i++)
        {
            data[i]->obj = createObj(data[i]->prev->obj.rect.x + TILE_SQUARE_SIDE, data[i]->obj.rect.y,
                                     TILE_SQUARE_SIDE, TILE_SQUARE_SIDE, FBM(freq, (data[i]->prev->obj.rect.x + TILE_SQUARE_SIDE) / TILE_SQUARE_SIDE, data[i]->obj.rect.y / TILE_SQUARE_SIDE, seed), 0);
            data[i] = data[i]->next;
        }
    }
    else if (x_out_of_fov == 1)
    {
        for (int i = 0; i < ROW_COUNT; i++)
        {
            data[i]->prev->obj = createObj(data[i]->obj.rect.x - TILE_SQUARE_SIDE, data[i]->obj.rect.y,
                                           TILE_SQUARE_SIDE, TILE_SQUARE_SIDE, FBM(freq, (data[i]->obj.rect.x - TILE_SQUARE_SIDE) / TILE_SQUARE_SIDE, data[i]->obj.rect.y / TILE_SQUARE_SIDE, seed), 0);
            data[i] = data[i]->prev;
        }
    }
    return 1;
}