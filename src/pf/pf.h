#ifndef __PF_HDR__
#define __PF_HDR__

#include <stdio.h>
#include "tinyoltp.h"

#define PFE_OK					0 		/* No error */
#define PFE_NOMEM 				1		/* No memory can be allocated */
#define PFE_NOBUF 				2		/* No buffer unit available */
#define PFE_PAGEFIXED 			3		/* Page pinned */
#define PFE_PAGENOTINBUF 		4		/* Page not in buffer */
#define PFE_INCOMPLETEREAD		5		/* Incomplete read */
#define PFE_INCOMPLETEWRITE 	6		/* Incomplete write */
#define PFE_HDRREAD				7		/* Error reading file header */
#define PFE_HDRWRITE			8		/* Error writing file header */
#define PFE_INVALIDPAGE			9		/* Invalid page number */
#define PFE_FILEOPEN			10		/* File open */
#define PFE_FTABFULL			11		/* File table full */
#define PFE_FD 					12		/* Invalid file descriptor */
#define PFE_EOF 				13		/* End of file */
#define PFE_PAGEFREE			14		/* Page not free */
#define PFE_PAGEUNFIXED 		15		/* Page is not pinned */

/* Paged file table. */
#define PF_FTAB_SIZE 20					/* Maximum number of entries in a PF 
										file table */
#define PF_PAGE_SIZE 256				/* Size of a page in characters
										(bytes) */

typedef struct PFhdr_str {
	int firstfree;	/* First free page in the linked list of free pages */
	int numpages; 	/* Number of pages in the file */
} PFhdr_str;

typedef struct PFfpage {
	int nextfree; 				/* PFE_PAGEFREE if not free, otherwise
									points to the next free page. */
	char *pagebuf;				/* Page data */
} PFfpage;

typedef struct PFftab_ele {
	boolean valid; 			/* Set to TRUE when the file is open */
	ino_t inode; 			/* Inode number of the file */
	char *fname; 			/* File name */
	int unixfd; 			/* Unix file descriptor */
	PFhdr_str hdr; 			/* File header */
	boolean hdrchanged; 	/* TRUE if file header has changed */
} PFftab_ele;

/* Buffer manager. */
#define PF_MAX_BUFS 		1000 	/* Maximum number of pages to allocate in
									the buffer */
#define PF_HASH_TBL_SIZE 	100 	/* Size of the hash table - i.e the number 
									of buckets in the hash table. */

typedef struct PFbpage {
	PFfpage *fpage; 			/* Page data from the file */
	/* The nextpage and prevpage pointers are used primarily to support the LRU page replacement strategy. */
	struct PFbpage *nextpage; 	/* Next in the linked list of buffer pages */
	struct PFbpage *prevpage; 	/* Previous in the linked list of buffer 
									pages */
	boolean dirty; 				/* TRUE if the page is dirty */
	short count; 				/* Pin count associated with the page */
	/* These values are mainly used to support the LRU page replacement strategy. */
	int pagenum; 				/* Page number of this page */
	int fd; 					/* PF file descriptor of this page
									Note: not to be confused with the UNIX file descriptor) */
} PFbpage;

typedef struct PFhash_entry {
	struct PFhash_entry *nextentry;	/* Next hash table element or NULL */
	struct PFhash_entry *preventry;	/* Prev hash table element or NULL */
	int fd;							/* File descriptor */
	int pagenum;					/* Page number */
	struct PFbpage *bpage;			/* Ptr to buffer holder this page */
} PFhash_entry;

void pf_init(void);
int pf_create_file(char *filename);
int pf_destroy_file(char *filename);
int pf_open_file(char *filename);
int pf_close_file(int fd);
int pf_get_next_page(int fd, int *pagenum, char *pagebuf);
int pf_get_first_page(int fd, int *pagenum, char *pagebuf);
int pf_get_this_page(int fd, int pagenum, char *pagebuf);
int pf_alloc_page(int fd, int *pagenum, char *pagebuf);
int pf_dispose_page(int fd, int pagenum);
int pf_dirty_page(int fd, int pagenum);
int pf_unfix_page(int fd, int pagenum);
int pf_unpin_page(int fd, int pagenum, int dirty);
int pf_rename_file(char *oldfile, char *newfile);
void pf_print_error(char *s);
void pf_set_err_stream(FILE *fp);

/* additional functions */
// void pf_denit(void);

/* should be private, put them into a separate header file. */
unsigned pf_hash(int fd, int pagenum);
PFhash_entry *lookup(int fd, int pagenum);
PFhash_entry *install(int fd, int pagenum, PFbpage *bpage);
long pf_pagenum_to_file_pos(int pagenum);
int pf_write_hdr(int fd, int firstfree, int numpages);
int pf_write_page(int fd, int pagenum, int nextfree, char *pagebuf);
PFhash_entry *pf_hash_lookup(int fd, int pagenum);
int pf_get_fd_of_filename(char *filename);


#endif