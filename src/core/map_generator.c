/*
    Used a techinque called Perlin Noise with Fractal Brownian Motion to generate the map.
    For reference here is the resource I used:
    https://rtouti.github.io/graphics/perlin-noise-algorithm
*/

#include "map_generator.h"

static float perlinNoise(float x, float y, float freq, int octaves, int* pP_table);
static int generate_pixel_colors(Uint32 pixel_colors[MAP_HEIGHT][MAP_WIDTH], float freq);
static int allocObj(Obj** terrain_map, int* pTerrain_map_i, int x, int y, int w,
                    int h, Uint32 color_hex, int can_collide);

int* initPTable() {
    int* table = malloc(P_TABLE_SIZE * sizeof(int));
    int temp, j;

    if (!table) {
        return NULL;
    }
    for (int i = 0; i < P_TABLE_SIZE; i++) {
        table[i] = i;
    }
    // Shuffling the array;
    for (int i = P_TABLE_SIZE - 1; i > 0; i --) {
        j = rand() % i;
        temp = table[i];
        table[i] = table[j];
        table[j] = temp;
    }
    return table;
}

int noise2(int x, int y, int* pP_table)
{
    int tmp = pP_table[(y + time(NULL)) % 256];
    return pP_table[(tmp + x) % 256];
}

float smooth_interpolate(float x, float y, float t)
{
    float ease_const = ((6 * t - 15) * t + 10) * t * t * t;
    return x + (y - x) * ease_const;
}

float noise2d(float x, float y, int* pP_table)
{
    int x_int = x;
    int y_int = y;
    x -= x_int;
    y -= y_int;
    int s = noise2(x_int, y_int, pP_table);
    int t = noise2(x_int+1, y_int, pP_table);
    int u = noise2(x_int, y_int+1, pP_table);
    int v = noise2(x_int+1, y_int+1, pP_table);
    float low = smooth_interpolate(s, t, x);
    float high = smooth_interpolate(u, v, x);
    return smooth_interpolate(low, high, y);
}



static float perlinNoise(float x, float y, float freq, int octaves, int* pP_table) {
    float amp = 1.0, fin = 0.0, div = 0.0;

    for(int i = 0; i < octaves; i++) {
        div += 256 * amp;
        fin += noise2d(x * freq, y * freq, pP_table) * amp;
        amp /= 2;
        freq *= 2;
    }
    return fin / div;
}

static int generate_pixel_colors(Uint32 pixel_colors[MAP_HEIGHT][MAP_WIDTH], float freq) {
    float n;
    int c;
    int* pP_table = initPTable();

    if (!pP_table) {
        return 0;
    }
    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            n = perlinNoise(x, y, freq, PERLIN_OCTAVES, pP_table);
            // Restircting the value of n to be between 0 and 1 instead of -1 and 1
            n += 1.0;
            n /= 2.0;
            n = n*n*n*n; // Improving the contrast
            c = (n * 255); // Convrting from [0, 1] to [0, 255]
            pixel_colors[y][x] = encodeColor(c, c, c, 255);
        }
    }
    free(pP_table);
    return 1;
}

void freeTerrainMap(Obj** terrain_map) {
    if (terrain_map) {
        for (int i = 0; terrain_map[i] != NULL; i++) {
            free(terrain_map[i]);
        }
        free(terrain_map);
    }
}


static int allocObj(Obj** terrain_map, int* pTerrain_map_i, int x, int y, int w,
                    int h, Uint32 color_hex, int can_collide) {
    Obj* obj = malloc(sizeof(Obj));

    if (!obj) {
        fprintf(stderr, "Can't malloc space for a object int the lvl data.\n");
        terrain_map[*pTerrain_map_i] = NULL;
        freeTerrainMap(terrain_map);
        return 0;
    }
    *obj = createObj(x, y, w, h, color_hex, can_collide);
    terrain_map[(*pTerrain_map_i)++] = obj;
    return 1;
}

Obj** generateTerrainMap() {
    int map_i = 0;
    Obj** terrain_map = malloc((MAP_HEIGHT* MAP_WIDTH + 1) * sizeof(Obj*)); // +1 for NULL sentinal
    Uint32 pixel_colors[MAP_HEIGHT][MAP_WIDTH];

    if (!terrain_map) {
        fprintf(stderr, "Failed to allocate memory for terrain map\n");
        return NULL;
    }
    if (!generate_pixel_colors(pixel_colors, PERLIN_TERRAIN_FREQ)) {
        freeTerrainMap(terrain_map);
        return NULL;
    }
    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            if (!allocObj(terrain_map, &map_i, x * TILE_SQUARE_SIDE, y * TILE_SQUARE_SIDE, TILE_SQUARE_SIDE, TILE_SQUARE_SIDE, pixel_colors[y][x], 0)) {
                return NULL;
            }
        }
    }
    terrain_map[map_i] = NULL;
    return terrain_map;
}