#ifndef __PF_HDR__
#define __PF_HDR__


#define PFE_OK					01 		/* No error */
#define PFE_NOMEM 				02		/* No memory can be allocated */
#define PFE_NOBUF 				04		/* No buffer unit available */
#define PFE_PAGEFIXED 			010		/* Page pinned */
#define PFE_PAGENOTINBUF 		020		/* Page not in buffer */
#define PFE_INCOMPLETEREAD		040		/* Incomplete read */
#define PFE_INCOMPLETEWRITE 	0100	/* Incomplete write */
#define PFE_HDRREAD				0200	/* Error reading file header */
#define PFE_HDRWRITE			0400	/* Error writing file header */
#define PFE_INVALIDPAGE			01000	/* Invalid page number */
#define PFE_FILEOPEN			02000	/* File open */
#define PFE_FTABFULL			04000	/* File table full */
#define PFE_FD 					010000	/* Invalid file descriptor */
#define PFE_EOF 				020000	/* End of file */
#define PFE_PAGEFREE 			040000	/* Page not free */
#define PFE_PAGEUNFIXED 		0100000 /* Page is not pinned */

void pf_init(void);
int pf_create_file(char *filename);
int pf_destroy_file(char *filename);
int pf_open_file(char *filename);
int pf_close_file(int fd);
int pf_get_next_page(int fd, int *pagenum, char **pagebuf);
int pf_get_first_page(int fd, int *pagenum, char **pagebuf);
int pf_get_this_page(int fd, int pagenum, char **pagebuf);
int pf_alloc_page(int fd, int *pagenum, char **pagebuf);
int pf_dispose_page(int fd, int pagenum);
int pf_dirty_page(int fd, int pagenum);
int pf_unfix_page(int fd, int pagenum);
int pf_unpin_page(int fd, int pagenum, int dirty);
int pf_rename_file(char *oldfile, char *newfile);
void pf_print_error(char *s);
void pf_set_err_stream(FILE *fp);


#endif