#include "level_parser.h"

static int allocObj(Obj** lvl_data, int* pLvl_data_i, int x, int y, int w, int h, int r, int g, int b, int a);

void freeLevelData(Obj** lvl_data) {
    for (int i = 0; lvl_data[i] != NULL; i++) {
        free(lvl_data[i]);
        lvl_data[i] = NULL;
    }
    free(lvl_data);
}

int reallocLevelData(Obj*** pLvl_data, int prev_size) {
    int new_size = prev_size + (prev_size / 2);
    Obj** temp = realloc(*pLvl_data, sizeof(Obj*) * (new_size + 1)); // So that we always have room for the NULL sentient value.

    if (!temp) {
        fprintf(stderr, "Failed to realloc lvl data array.\n");
        return 0;
    }
    *pLvl_data = temp;
    return new_size;
}

static int allocObj(Obj** lvl_data, int* pLvl_data_i, int x, int y, int w, int h, int r, int g, int b, int a) {
    Obj* obj = malloc(sizeof(Obj));

    if (!obj) {
        fprintf(stderr, "Can't malloc space for a object int the lvl data.\n");
        lvl_data[*pLvl_data_i] = NULL;
        freeLevelData(lvl_data);
        return 0;
    }
    *obj = createObj(x, y, w, h, r, g, b, a);
    lvl_data[(*pLvl_data_i)++] = obj;
    return 1;
}

Obj** parseLevel(int lvl_number) {
    char lvl_path[LVL_PATH_SIZE], buff[BUFF_SIZE]; 
    snprintf(lvl_path, sizeof(lvl_path), "levels/lvl%d.txt", lvl_number);
    FILE* lvl_file = fopen(lvl_path, "r");
    int lvl_data_size = LVL_MIN_COL * LVL_MIN_ROW;
    Obj** lvl_data = malloc(sizeof(Obj*) * (lvl_data_size + 1)); // The +1 leaves room for the ending NULL sentient value if all rest are filled.
    int line_c = 0, lvl_data_i = 0;

    if (!lvl_file) {
        fprintf(stderr, "Can't open the level file at the path: %s\n", lvl_path);
        return NULL;
    }
    if (!lvl_data) {
        fprintf(stderr, "Can't malloc space for the lvl_data of the level file at the path: %s\n", lvl_path);
        fclose(lvl_file);
        return NULL;
    }
    while (fgets(buff, sizeof(buff), lvl_file)) {
        for (int i = 0; buff[i] != '\n' && buff[i] != '\0'; i++) {
            if (buff[i] == ' ') {
                continue;
            }
            if (lvl_data_i == lvl_data_size) {
                lvl_data_size = reallocLevelData(&lvl_data, lvl_data_size);
            }
            if (!allocObj(lvl_data, &lvl_data_i, i * LVL_TO_SCREEN, line_c * LVL_TO_SCREEN, LVL_TO_SCREEN,
                     LVL_TO_SCREEN, 100, 100, 100, 255)) {
                fclose(lvl_file);
                return NULL;
            }
        }
        line_c++;
    }
    fclose(lvl_file);
    lvl_data[lvl_data_i] = NULL;
    return lvl_data;
}