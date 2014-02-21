#include '../tinyrel.h'

#define HFE_OK				 0 /* HF function successful */
#define HFE_PF				-1 /* error in PF layer */
#define HFE_FTABFULL		-2 /* files open exceeds MAXOPENFILES */
#define HFE_STABFULL		-3 /* # scans open exceeds MAXSCANS */
#define HFE_FD				-4 /* invalid file descriptor */
#define HFE_SD				-5 /* invalid scan descriptor */
#define HFE_INVALIDRECORD	-6 /* invalid record id */
#define HFE_EOF				-7 /* end of file */

void hf_init(void);
int hf_create_file(char *filename, int recsize);
int hf_destroy_file(char *filename);
int hf_open_file(char *filename);
int hf_close_file(int filedesc);
RECID hf_insert_rec(int filedesc, char *record);P
int hf_delete_rec(int filedesc, RECID recid);
RECID hf_get_first_rec(int filedesc, char *record);
RECID hf_get_next_rec(int filedesc, RECID recid, char *record);
int hf_get_this_rec(int filedesc, RECID recid, char *record);
int hf_open_file_scan(int filedesc, char attrtype, int attrlength, int attroffset, int op, char *value);
RECID hf_find_next_rec(int scandesc, char *record);
int hf_close_file_scan(int scandesc);
void hf_print_error(char *errstring);
bool_t hf_valid_rec_id(int filedesc, RECID recid);