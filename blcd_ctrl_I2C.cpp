#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include "blcd_ctrl_I2C.h"
#include <Arduino.h>

/*
 * TODO: implement blcd_write_at_pos function wiht the BLCD_EMPTY_CHAR 
 */

int
blcd_write(arrc_t*** array, const char* buff) {
    if (buff == NULL) {
        return -1;
    }
    cpos_t cursor_pos = blcd_get_cur(*array);
    if (!blcd_cmp(cursor_pos, BLCD_SEEK_UNDEF)) {
        return -1;
    }
    cpos_t crs_pos = blcd_get_cur(*array);
    char* str = strdup(buff);
    for (int i = 0; i < strlen(str); ++i) {
        memset(array[crs_pos.line][crs_pos.coll] + i, str[i], 1);
    }
    return 1;
}

int
blcd_write_at_pos(arrc_t*** array, cpos_t curr_pos, const char* buff) {

}

int
blcd_update_cur(arrc_t*** array, cpos_t* curr_pos, const char* buff) {
    cpos_t curr = blcd_get_cur(*array);
    if (curr.line == -1 && curr.coll == -1) {
        blcd_set_cur(array, BLCD_SEEK_START, curr_pos);
        return 0;
    } else {
        int empty_colls = blcd_count_empty_colls(*array, curr.line);
        int used_colls = DPY_COLLS - empty_colls;
        int result = blcd_unset_cur(array);
        if (result == -1) {
            return result;
        }
        int new_coll_pos = curr.coll + (int)strlen(buff);
        result = blcd_set_cur(array, (cpos_t){ curr.line, new_coll_pos }, curr_pos);
        if (result == -1) {
            return result;
        }
        return 1;
    }
}

int
blcd_set_cur(arrc_t*** array, cpos_t set_pos, cpos_t* cur_pos) {
    cpos_t curr_pos = blcd_get_cur(*array);
    if (blcd_cmp(set_pos, *cur_pos) != 0) {
        array[set_pos.line][set_pos.coll]->crs_present = true;
        cur_pos->line = set_pos.line;
        cur_pos->coll = set_pos.coll;
        return 1;
    }
    return -1;
}

int blcd_isset_cur(arrc_t** array) {
    cpos_t cur_pos = blcd_get_cur(array);
    int res = 0;
    if (!blcd_cmp(cur_pos, BLCD_SEEK_UNDEF)) {
        return 0;
    } else if (blcd_cmp(cur_pos, BLCD_SEEK_UNDEF) > 0) {
        return 1;
    }
    return -1;
}

int blcd_cmp(cpos_t first_cur_pos, cpos_t sec_cur_pos) {
    if (first_cur_pos.line == sec_cur_pos.line && first_cur_pos.coll == sec_cur_pos.coll) {
        return 0;
    } else if (first_cur_pos.line > sec_cur_pos.line && first_cur_pos.coll > sec_cur_pos.coll) {
        return 1;
    }
    return 2;
}

int
blcd_unset_cur(arrc_t*** array) {
    cpos_t curr_pos = blcd_get_cur(*array);
    if (!blcd_cmp(curr_pos, BLCD_SEEK_UNDEF)) {
        return -1;
    }
    array[curr_pos.line][curr_pos.coll]->crs_present = false;
    return 1;
}

cpos_t
blcd_get_cur(arrc_t** array) {
    cpos_t current_cursor_pos = BLCD_SEEK_UNDEF;
    for (int l = 0; l < DPY_LINES; ++l) {
        for (int c = 0; c < DPY_COLLS; ++c) {
            if (array[l][c].crs_present) {
                current_cursor_pos.line = l;
                current_cursor_pos.coll = c;
                break;
            }
        }
    }
    return current_cursor_pos;
}

char*
blcd_read_line(arrc_t** array, int line) {
    int count = 0;
    for (int i = 0; i < DPY_COLLS; ++i) {
        if (!isprint(array[line][i].c)) {
            count++;
        }
    }
    char* line_str = (char*)malloc((count + 1) * sizeof(char));
    for (int x = 0; x < count; ++x) {
        memset(&line_str[x], array[line][x].c, 1);
    }
    memset(&line_str[count + 1], '\0', 1);
    return line_str;
}

char
blcd_read_char(arrc_t** array, cpos_t char_pos) {
    char c = array[char_pos.line][char_pos.coll].c;
    if (c == 0 || c == BLCD_EMPTY_CHAR) {
        return -1;
    }
    return c;
}

cpos_t
blcd_find_char_line(arrc_t** array, char c, int line) {
    cpos_t char_pos = BLCD_SEEK_UNDEF;
    for (int i = 0; i < DPY_COLLS; ++i) {
        if (array[line][i].c == c) {
            char_pos = { line, i};
        }
    }
    return char_pos;
}

cpos_t
blcd_alloc(arrc_t*** arr, size_t nmemb, size_t memb_size) {
    *arr = (arrc_t**)calloc(nmemb, sizeof(arrc_t*));
    for (size_t i = 0; i < nmemb; i++) {
        (*arr)[i] = (arrc_t*)calloc(memb_size, sizeof(arrc_t));
    }
    /* Set initial cursor position to { -1, -1 } so undefined */
    return BLCD_SEEK_UNDEF;
}

void 
blcd_alloc_line(arrc_t*** arr, int line, size_t memb_size) {
    (*arr)[line] = (arrc_t*)calloc(memb_size, sizeof(arrc_t));
}

void
blcd_free(arrc_t*** arr, size_t nmemb) {
    for (size_t i = 0; i < nmemb; i++) {
        free((*arr)[i]);
    }
    free(*arr);
}

void
blcd_free_line(arrc_t*** arr, int line) {
    free((*arr)[line]);
}

cpos_t
blcd_clear(arrc_t*** array) {
    blcd_free(array, DPY_LINES);
    blcd_alloc(array, DPY_LINES, DPY_COLLS);
}

void
blcd_clear_line(arrc_t*** array, int line) {
    blcd_free_line(array, DPY_LINES);
    blcd_alloc_line(array, line, DPY_COLLS);
}

int
blcd_count_empty_lines(arrc_t** array) {
    int ccount = 0, lcount = 0;
    for (int l = 0; l < DPY_LINES; ++l) {
        for (int c = 0; c < DPY_COLLS; ++c) {
            if (array[l][c].c == 0 || array[l][c].c == BLCD_EMPTY_CHAR) {
                ccount++;
            }
        }
        if (ccount == 20) {
            return lcount;
        }
        lcount++;
    }
}

int
blcd_count_empty_colls(arrc_t** array, int line) {
    int ccount = 0;
    for (int c = 0; c < DPY_COLLS; ++c) {
        if (array[line][c].c == 0) {
            ccount++;
        }
    }
    return ccount;
}
