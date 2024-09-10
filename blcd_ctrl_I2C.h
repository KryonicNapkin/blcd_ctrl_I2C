#if !defined(BLCD_CTRL_I2C_H)
#define BLCD_CTRL_I2C_H

#include <stdio.h>
#include "Print.h"

typedef struct {
    int line;
    int coll;
} cpos_t;

typedef struct {
    char c;
    bool crs_present;
} arrc_t;

/* Automaticaly library will assume you are using 16x2 display */
#if !defined(DPY_COLLS)
#define DPY_COLLS 16
#endif
#if !defined(DPY_LINES)
#define DPY_LINES 2
#endif

#define BLCD_EMPTY_CHAR   '\xa0'

#define BLCD_SEEK_UNDEF   (cpos_t){ -1, -1 }
#define BLCD_SEEK_START   (cpos_t){ 0, 0 }
#define BLCD_SEEK_END     (cpos_t){ DPY_LINES - 1, DPY_COLLS - 1 }
#define BLCD_SEEK_LEFT_D  (cpos_t){ DPY_LINES - 1, 0 }
#define BLCD_SEEK_RIGHT_U (cpos_t){ 0, DPY_COLLS - 1 }

#define DPY_SIZE(l, c) ((DPY_LINES) * (DPY_COLLS))

int blcd_write(arrc_t*** array, const char* buff);
int blcd_write_at_cur(arrc_t*** array, cpos_t curr_pos, const char* buff);
int blcd_update_cur(arrc_t*** array, cpos_t* curr_pos, const char* buff);
int blcd_set_cur(arrc_t*** array, cpos_t pos, cpos_t* curr_pos);
int blcd_isset_cur(arrc_t** array);
int blcd_cmp(cpos_t first_cur_pos, cpos_t sec_cur_pos);
int blcd_unset_cur(arrc_t*** array);
cpos_t blcd_get_cur(arrc_t** array);
char* blcd_read_line(arrc_t** array, int line);
char blcd_read_char(arrc_t** array, cpos_t char_pos);
cpos_t blcd_find_char_line(arrc_t** array, char c, int line);
cpos_t blcd_alloc(arrc_t*** arr, size_t nmemb, size_t memb_size);
void blcd_alloc_line(arrc_t*** arr, int line, size_t memb_size);
void blcd_free(arrc_t*** arr, size_t nmemb);
void blcd_free_line(arrc_t*** arr, int line);
cpos_t blcd_clear(arrc_t*** array);
void blcd_clear_line(arrc_t*** array, int line);
int blcd_count_empty_lines(arrc_t** array);
int blcd_count_empty_colls(arrc_t** array, int line);

#endif
