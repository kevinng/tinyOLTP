#include '../tinyrel.h'

#define EQ_OP	1
#define LT_OP	2
#define GT_OP	3
#define LE_OP	4
#define GE_OP	5
#define NE_OP	6

void am_init(void);
int am_create_index(char *filename, int indexno, char attrtype, int attrlength, bool_t isunique);
int am_destroy_index(char *filename, int indexno);
int am_open_index(char *filename, int indexno);
int am_close_index(int AM_fd);
int am_insert_entry(int AM_fd, char *value, RECID recid);
int am_delete_entry(int AM_fd, char *value, RECID recid);
int am_open_index_scan(int AM_fd, int op, char *value);
RECID am_find_next_entry(int scandesc);
int am_close_index_scan(int scandesc);
void am_print_error(char *errstring);
