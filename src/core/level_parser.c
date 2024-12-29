#include "level_parser.h"

static int allocObj(Obj** lvl_data, int* pLvl_data_i, int x, int y, int w,
                    int h, int r, int g, int b, int a, int can_collide);
static int reallocLvlData(Obj*** pLvl_data, int prev_size);
static Obj** parseTilemap(int lvl_number);

void freeLvlTileset(Obj** lvl_data) {
    for (int i = 0; lvl_data[i] != NULL; i++) {
        free(lvl_data[i]);
        lvl_data[i] = NULL;
    }
    free(lvl_data);
}

static int reallocLvlData(Obj*** pLvl_data, int prev_size) {
    int new_size = prev_size + (prev_size / 2);
    Obj** temp = realloc(*pLvl_data, sizeof(Obj*) * (new_size + 1)); // So that we always have room for the NULL sentient value we add a +1.

    if (!temp) {
        fprintf(stderr, "Failed to realloc lvl data array.\n");
        return 0;
    }
    *pLvl_data = temp;
    return new_size;
}

static int allocObj(Obj** lvl_data, int* pLvl_data_i, int x, int y, int w,
                    int h, int r, int g, int b, int a, int can_collide) {
    Obj* obj = malloc(sizeof(Obj));

    if (!obj) {
        fprintf(stderr, "Can't malloc space for a object int the lvl data.\n");
        lvl_data[*pLvl_data_i] = NULL;
        freeLvlTileset(lvl_data);
        return 0;
    }
    *obj = createObj(x, y, w, h, r, g, b, a, can_collide);
    lvl_data[(*pLvl_data_i)++] = obj;
    return 1;
}

static Obj** parseTilemap(int lvl_number) {
    char lvl_dir_path[LVL_PATH_SIZE];
    snprintf(lvl_dir_path, sizeof(lvl_dir_path), "levels/lvl%d/tilemap.json", lvl_number);
    struct json_object *tilemap_obj = json_object_from_file(lvl_dir_path), *color_obj,
                       *tile_size_obj, *pos_matrix_obj, *properties_obj, *pos;

    if (!tilemap_obj) {
        fprintf(stderr, "Can't open or parse the tilemap file at the path: %s\n", lvl_dir_path);
        return NULL;
    }

    int lvl_data_size = MIN_TILE_SIZE;
    Obj** lvl_data = malloc(sizeof(Obj*) * (lvl_data_size + 1)); // +1 for NULL sentinel

    if (!lvl_data) {
        fprintf(stderr, "Can't malloc space for the lvl data array.\n");
        json_object_put(tilemap_obj);
        return NULL;
    }

    int lvl_data_i = 0, w_index, h_index, x_index, y_index, can_collide;
    Uint8 r, g, b, a;

    // NOTE FOR DEV: This is a fucking macro, stop thinking it can't happen.
    json_object_object_foreach(tilemap_obj, key, val) {
        // Decoding of each individual tile.
        color_obj = json_object_object_get(val, "color");
        tile_size_obj = json_object_object_get(val, "tile_size_index");
        pos_matrix_obj = json_object_object_get(val, "pos_index_matrix");
        properties_obj = json_object_object_get(val, "properties");
        can_collide = json_object_get_int(json_object_object_get(properties_obj, "collision"));
        r = json_object_get_int(json_object_array_get_idx(color_obj, 0));
        g = json_object_get_int(json_object_array_get_idx(color_obj, 1));
        b = json_object_get_int(json_object_array_get_idx(color_obj, 2));
        a = json_object_get_int(json_object_array_get_idx(color_obj, 3));
        w_index = json_object_get_int(json_object_array_get_idx(tile_size_obj, 0));
        h_index = json_object_get_int(json_object_array_get_idx(tile_size_obj, 1));
        for (int i = 0; i < json_object_array_length(pos_matrix_obj); i++) {
            pos = json_object_array_get_idx(pos_matrix_obj, i);
            x_index = json_object_get_int(json_object_array_get_idx(pos, 0));
            y_index = json_object_get_int(json_object_array_get_idx(pos, 1));

            if (lvl_data_i == lvl_data_size) {
                lvl_data_size = reallocLvlData(&lvl_data, lvl_data_size);
            }
            if (!allocObj(lvl_data, &lvl_data_i, x_index * TILE_SQUARE_SIDE, y_index * TILE_SQUARE_SIDE,
                          w_index * TILE_SQUARE_SIDE, h_index * TILE_SQUARE_SIDE, r, g, b, a, can_collide)) {
                json_object_put(tilemap_obj);
                return NULL;
            }
        }
    }
    lvl_data[lvl_data_i] = NULL; // Sentiel val.
    json_object_put(tilemap_obj);
    return lvl_data;
}

void parseLvl(int lvl_number, Obj*** pLvl_tileset) {
    char lvl_dir_path[LVL_PATH_SIZE];
    snprintf(lvl_dir_path, sizeof(lvl_dir_path), "levels/lvl%d", lvl_number);
    DIR* lvl_dir = opendir(lvl_dir_path);
    struct dirent* entry;

    if (!lvl_dir) {
        fprintf(stderr, "Can't open the level directory at the path: %s\n", lvl_dir_path);
        closedir(lvl_dir);
        *pLvl_tileset = NULL; //Note other data that came here to be initialized shall be nullified here.
        return;
    }
    while ((entry = readdir(lvl_dir))) {
        if (!strcmp(entry->d_name, ".") || !strcmp(entry->d_name, "..")) {
            continue;
        }
        if (!strcmp(entry->d_name, "tilemap.json")) {
            *pLvl_tileset = parseTilemap(lvl_number);
        }
    }
    closedir(lvl_dir);
}
