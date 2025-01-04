/*
    Used a techinque called Perlin Noise with Fractal Brownian Motion to generate the map.
    For reference here is the resource I used:
    https://rtouti.github.io/graphics/perlin-noise-algorithm
*/

#include "map_manager.h"

static int noise(int x, int y, int *pP_table, int seed);
static float smoothInterpolation(float x, float y, float t);
static float noise2D(float x, float y, int *pP_table, int seed);
static float perlinNoise(float x, float y, float freq, int octaves, int *pP_table, int seed);
static Uint32 FBM(float freq, int x, int y, int *pP_table, int seed);
static void freeColData(ScreenColData *to_free);

static int noise(int x, int y, int *pP_table, int seed)
{
    int tmp = pP_table[(y + seed) % P_TABLE_SIZE];

    return pP_table[abs((tmp + x) % P_TABLE_SIZE)];
}

static float smoothInterpolation(float x, float y, float t)
{
    float ease_const = ((6 * t - 15) * t + 10) * t * t * t;

    return x + (y - x) * ease_const;
}

static float noise2D(float x, float y, int *pP_table, int seed)
{
    int x_int = x, y_int = y;

    x -= x_int;
    y -= y_int;
    int s = noise(x_int, y_int, pP_table, seed);
    int t = noise(x_int + 1, y_int, pP_table, seed);
    int u = noise(x_int, y_int + 1, pP_table, seed);
    int v = noise(x_int + 1, y_int + 1, pP_table, seed);
    float low = smoothInterpolation(s, t, x);
    float high = smoothInterpolation(u, v, x);
    return smoothInterpolation(low, high, y);
}

static float perlinNoise(float x, float y, float freq, int octaves, int *pP_table, int seed)
{
    float amp = 1.0, fin = 0.0, div = 0.0;

    for (int i = 0; i < octaves; i++)
    {
        div += 256 * amp;
        fin += noise2D(x * freq, y * freq, pP_table, seed) * amp;
        amp /= 2;
        freq *= 2;
    }
    return fin / div;
}

static Uint32 FBM(float freq, int x, int y, int *pP_table, int seed)
{
    float n;

    n = perlinNoise(x, y, freq, PERLIN_OCTAVES, pP_table, seed);
    // Restircting the value of n to be between 0 and 1 instead of -1 and 1
    n += 1.0;
    n /= 2.0;
    n = n * n * n * n;                // Improving the contrast
    if (freq <= 0.02 && freq >= 0.01) // Due to floating point inaccuracy.
    {
        if (n <= 0.2)
        {
            return encodeColor(123, 232, 33, 255);
        }
        else if (n <= 0.4)
        {
            return encodeColor(118, 182, 196, 255);
        }
        else if (n <= 0.6)
        {
            return encodeColor(194, 178, 128, 255);
        }
        else if (n <= 0.8)
        {
            return encodeColor(124, 252, 0, 255);
        }
        else
        {
            return encodeColor(250, 250, 250, 255);
        }
    }
    return encodeColor(255, 255, 255, 255);
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

int *initPTable()
{
    int *table = malloc(P_TABLE_SIZE * sizeof(int));
    int temp, j;

    if (!table)
    {
        return NULL;
    }
    for (int i = 0; i < P_TABLE_SIZE; i++)
    {
        table[i] = i;
    }
    // Shuffling the array;
    for (int i = P_TABLE_SIZE - 1; i > 0; i--)
    {
        j = rand() % i;
        temp = table[i];
        table[i] = table[j];
        table[j] = temp;
    }
    return table;
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

ScreenColData **initScreenData(int *pP_table, float freq, int seed)
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
                            TILE_SQUARE_SIDE, TILE_SQUARE_SIDE, 0, FBM(freq, j, i, pP_table, seed)))
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

int updateDataOutOfFOV(ScreenColData **data, int x_out_of_fov, int y_out_of_fov, float freq, int *pP_table, int seed)
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
                            TILE_SQUARE_SIDE, TILE_SQUARE_SIDE, 0, FBM(freq, curr->obj.rect.x / TILE_SQUARE_SIDE, (curr->obj.rect.y + TILE_SQUARE_SIDE) / TILE_SQUARE_SIDE, pP_table, seed)))
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
                            TILE_SQUARE_SIDE, TILE_SQUARE_SIDE, 0, FBM(freq, curr->obj.rect.x / TILE_SQUARE_SIDE, (curr->obj.rect.y - TILE_SQUARE_SIDE) / TILE_SQUARE_SIDE, pP_table, seed)))
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
                                     TILE_SQUARE_SIDE, TILE_SQUARE_SIDE, FBM(freq, (data[i]->prev->obj.rect.x + TILE_SQUARE_SIDE) / TILE_SQUARE_SIDE, data[i]->obj.rect.y / TILE_SQUARE_SIDE, pP_table, seed), 0);
            data[i] = data[i]->next;
        }
    }
    else if (x_out_of_fov == 1)
    {
        for (int i = 0; i < ROW_COUNT; i++)
        {
            data[i]->prev->obj = createObj(data[i]->obj.rect.x - TILE_SQUARE_SIDE, data[i]->obj.rect.y,
                                           TILE_SQUARE_SIDE, TILE_SQUARE_SIDE, FBM(freq, (data[i]->obj.rect.x - TILE_SQUARE_SIDE) / TILE_SQUARE_SIDE, data[i]->obj.rect.y / TILE_SQUARE_SIDE, pP_table, seed), 0);
            data[i] = data[i]->prev;
        }
    }
    return 1;
}