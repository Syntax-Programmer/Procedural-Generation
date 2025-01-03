/*
    Used a techinque called Perlin Noise with Fractal Brownian Motion to generate the map.
    For reference here is the resource I used:
    https://rtouti.github.io/graphics/perlin-noise-algorithm
*/

#include "map_generator.h"

static int noise(int x, int y, int *pP_table);
static float smoothInterpolation(float x, float y, float t);
static float noise2D(float x, float y, int *pP_table);
static float perlinNoise(float x, float y, float freq, int octaves, int *pP_table);
static float FBM(float freq, int x, int y, int *pP_table);
static void freeColData(ScreenColData *to_free);

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

static int noise(int x, int y, int *pP_table)
{
    int tmp = pP_table[(y + time(NULL)) % 256];

    return pP_table[(tmp + x) % 256];
}

static float smoothInterpolation(float x, float y, float t)
{
    float ease_const = ((6 * t - 15) * t + 10) * t * t * t;

    return x + (y - x) * ease_const;
}

static float noise2D(float x, float y, int *pP_table)
{
    int x_int = x, y_int = y;

    x -= x_int;
    y -= y_int;
    int s = noise(x_int, y_int, pP_table);
    int t = noise(x_int + 1, y_int, pP_table);
    int u = noise(x_int, y_int + 1, pP_table);
    int v = noise(x_int + 1, y_int + 1, pP_table);
    float low = smoothInterpolation(s, t, x);
    float high = smoothInterpolation(u, v, x);
    return smoothInterpolation(low, high, y);
}

static float perlinNoise(float x, float y, float freq, int octaves, int *pP_table)
{
    float amp = 1.0, fin = 0.0, div = 0.0;

    for (int i = 0; i < octaves; i++)
    {
        div += 256 * amp;
        fin += noise2D(x * freq, y * freq, pP_table) * amp;
        amp /= 2;
        freq *= 2;
    }
    return fin / div;
}

static float FBM(float freq, int x, int y, int *pP_table)
{
    float n;

    n = perlinNoise(x, y, freq, PERLIN_OCTAVES, pP_table);
    // Restircting the value of n to be between 0 and 1 instead of -1 and 1
    n += 1.0;
    n /= 2.0;
    n = n * n * n * n; // Improving the contrast
    return n;
}

static void freeColData(ScreenColData *to_free)
{
    ScreenColData *next;

    if (!to_free)
    {
        return;
    }
    for (int i = 0; i < COL_COUNT; i++)
    {
        next = to_free->next;
        free(to_free);
        to_free = next;
    }
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

ScreenColData **initScreenData(int *pP_table, float freq)
{
    ScreenColData **screen_data = malloc(ROW_COUNT * sizeof(ScreenColData *)); // NOTE: This ScreenColData** is not equivalent to the screeColData** in addColNode()
    float perlin_index;
    Uint32 color = 0;

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
            perlin_index = FBM(freq, j, i, pP_table);
            if (freq <= 0.02 && freq >= 0.01) // Due to floating point inaccuracy.
            {
                if (perlin_index <= 0.2)
                {
                    color = encodeColor(100, 100, 100, 255);
                }
                else if (perlin_index <= 0.4)
                {
                    color = encodeColor(150, 150, 150, 255);
                }
                else if (perlin_index <= 0.6)
                {
                    color = encodeColor(200, 200, 200, 255);
                }
                else
                {
                    color = encodeColor(250, 250, 250, 255);
                }
            }
            if (!addColNode(&screen_data[i], 0, j * TILE_SQUARE_SIDE, i * TILE_SQUARE_SIDE,
                            TILE_SQUARE_SIDE, TILE_SQUARE_SIDE, 0, color))
            {
                freeScreenData(screen_data);
                return NULL;
            }
        }
    }
    return screen_data;
}