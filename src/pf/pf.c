#include <stdio.h>
#include <sys/stat.h>
#include "pf.h"
#include "tinyrel.h"

#define PF_FTAB_SIZE 1024	/* maximum number of entries in a PF file table */
#define PF_PAGE_SIZE 256	/* size of a page in characters (bytes) */

typedef struct PFhdr_str {
	int firstfree; /* first free page in the linked list of free pages */
	int numpages; /* number of pages in the file */
} PFhdr_str;

typedef struct PFfpage {
	int nextfree; /* (1) free or not (2) points to the next free page */
	char pagebuf[PF_PAGE_SIZE];
} PFfpage;

typedef struct PFftab_ele {
	boolean valid; /* set to TRUE when the file is open */
	ino_t inode; /* inode number of the file */
	char *fname; /* file name */
	int unixfd; /* Unix file descriptor */
	PFhdr_str hdr; /* file header */
	short hdrchanged; /* TRUE if file header has changed */
} PFftab_ele;

#define PF_MAX_BUFS ? /* Maximum number of pages to allocate in the buffer */

typedef struct PFbpage {
	PFfpage fpage; /* page data from the file */
	struct PFbpage *nextpage; /* next in the linked list of buffer pages */
	struct PFbpage *prevpage; /* previous in the linked list of buffer pages */
	boolean dirty; /* TRUE if the page is dirty */
	short count; /* pin count associated with the page */
	int pagenum; /* page number of this page */
	int fd; /* PF file descriptor of this page (note: not to be confused with the UNIX file descriptor) */
} PFbpage;

static PFbpage *PFfirstbpage = NULL; /* ptr to first buffer page, or NULL */
static PFbpage *PFlastbpage = NULL; /* ptr to last buffer page, or NULL */
static PFbpage *PFfreebpage = NULL; /* list of free buffer page */

#define PF_HASH_TBL_SIZE ? /* Size of the hash table */

typedef struct PFhash_entry {
	struct PFhash_entry *nextentry; /* next hash table element or NULL */
	struct PFhash_entry *preventry; /* prev hash table element or NULL */
	int fd; /* file descriptor */
	int pagenum; /* page number */
	struct PFbpage *bpage; /* ptr to buffer holder this page */
} PFhash_entry;

/*
This function initializes the PF layer. It also initializes the error stream (to stderr). This must be the first function to call in order to use the PF layer. No return value.
 */
void pf_init()
{

}

/*
This function creates a file named filename. This file should not have already existed before. The system call open() is used to create the file. The PF file header is initialized (for firstfree and numpages). This PF header information is written to the file by using the write() system call. After the header has been written, the file is closed using the close() system call. This function returns PFE_OK if the operation is successful, an error condition otherwise.

char *filename		- name of the file to be created
  */
int pf_create_file(char *filename)
{
	return -1;
}

/*
This function destroys the file filename. The file should exist, and should not be already open. This function returns PFE_OK if the operation is succ
essful.

char *filename		- name of the file to be destroyed
 */
int pf_destroy_file(char *filename)
{
	return -1;
}

/*
This function opens the file named filename using the system call open(). The file header is read and the inode of the file is obtained by using the system call fstat(). The fields in the file table entry are filled accordingly and the index of the file table entry is returned. (This is the PF file descriptor.) It is possible to open a file more than once, however, it will be treated as 2 separate files (different file descriptors, different buffers). Thus, opening a file more than once for writing may corrupt the file, and can, in certain circumstances, crash the PF layer. Note that even if only one instance of a file is for writing, problems may occur because some writes may not be seen by a reader of another instance of the file. This function returns a PF file descriptor if the operation is successful, an error condition if otherwise.

char *filename		- name of the file to be opened
 */
int pf_open_file(char *filename)
{
	return -1;
}

/*
This function closes the file associated with the PF file descriptor fd. The buffers of the file are flushed (meaning the dirty pages are written back). If the file header has changed, it is written back to the file. The file is finally closed by using the system call close(). The file table entry corresponding to that file is made invalid. This function returns PFE_OK if the operation is successful, an error condition otherwise.

int fd 				- PF file descriptor
 */
int pf_close_file(int fd)
{
	return -1;
}

/*
This function gets the first valid page (page being used, either empty or not) after the page represented by pagenum in the file represented by fd. A linear scan of the file is done and each page is retrieved. If a page is retrieved is not valid, then it is unpinned and the next page is read. The pagenum argument is used to return the page number of the next valid page. The pagebug arguments points to the content of the PF data page. This function returns PFE_OK if the operation is successful, PFE_EOF if the end of file is reached without finding any used page data, PFE_INVALIDPAGE if page is invalid, a PF error code otherwise.

int fd 				- PF file descriptor
int *pagenum		- input starting page number, returns page number of the 
	first page after the starting page number
char **pagebuf		- return data of the page
 */
int pf_get_next_page(int fd, int *pagenum, char **pagebuf)
{
	return -1;
}

/*
This function gets the first valid page in the file associated with file descriptor fd. This function is implemented using pf_get_next_page(). (A pagenum of -1 is passed to pf_get_next_page() to indicate to start looking for a valid page from the beginning of the file). It returns PFE_OK if the operation is successful, PFE_EOF if the end of file is reached without finding any used page data, a PF error code otherwise.

int fd 				- PF file descriptor
int *pagenum		- return page number of the first page
char **pagebuf		- return data of the page
 */
int pf_get_first_page(int fd, int *pagenum, char **pagebuf)
{
	return -1;
}

/*
This function reads a valid page specified by pagenum from the file associated with file descriptor fd, and sets *pagebuf to point to the page data. If the page specified by pagenum is not valid, then the retrieved page is unpinned, and an error code PFE_INVALIDPAGE is returned. It returns PFE_OK if the page is valid and the operation is successful, and a PF error code otherwise.

int fd 				- PF file descriptor
int pagenum 		- page number of the page to retrieve
char **pagebuf		- return data of the page
 */
int pf_get_this_page(int fd, int pagenum, char **pagebuf)
{
	return -1;
}

/*
This function allocates a page in the file associated with a file descriptor fd. If there is a free page in the file, the free page is reused without adding a new page to the file. The content of the free page is read into the a buffer page. Note that only valid information from this free page is the nextfree field. Henceforth, this page is considered valid, until it is disposed again. If there is no free page in the file, then a new valid page is added to the file. In both cases, the value of pagenum for the page being allocated by this function must be determined (from the information stored in the file head). In both cases, the page allocated by this function is pinned and marked dirty so that it will be written to the file eventually. This function returns PFE_OK if the operation is successful, an error condition otherwise.

int fd 				- PF file descriptor
int *pagenum 		- return page number of the page allocated
char **pagebuf		- return data of the page
 */
int pf_alloc_page(int fd, int *pagenum, char **pagebuf)
{
	return -1;
}

/*
This function disposes the page pagenum of the file with file descriptor fd, converting a valid page into a free page. The approach to dispose a page is to bring the page to the buffer (if it is not already there) and change its nextfree field to point to the beginning of the list of free pages associated with the file. Note that the page is brought into memory, only because the nextfree field is kept in a disk page. The first element of the list of free pages associated with the file will be the most recently disposed page. The page is marked dirty in the buffer so that change is sent eventually to the file in the disk. Only a page whose copy in the buffer is unpinned can be disposed. This function returns PFE_OK if the operation is successful, an error condition otherwise.

int fd 				- PF file descriptor
int pagenum 		- page number of the page to be disposed
 */
int pf_dispose_page(int fd, int pagenum)
{
	return -1;
}

/*
This function marks the pagenum of the file referenced by fd dirty. It returns PFE_OK if the operation is successful, an error condition otherwise.

int fd 				- PF file descriptor
int pagenum			- page number of the page to be marked dirty
 */
int pf_dirty_page(int fd, int pagenum)
{
	return -1;
}

/*
After checking the validity of the fd and pagenum values, this function unpins the file numbered pagenum of the file with the file descriptor fd. This function returns PFE_OK if the operation is successful, an error condition otherwise.

int fd 				- PF file descriptor
int pagenum			- page number of the page to be unpinned
 */
int pf_unfix_page(int fd, int pagenum)
{
	return -1;
}

/*
If the dirty parameter if TRUE, then it first marks the page dirty before it is unpinned. Otherwise, the effect of this function is identical to that of pf_unfix_page().

int fd				- PF file descriptor
int pagenum			- page number of the page to be unpinned
int dirty 			- dirty indication
 */
int pf_unpin_page(int fd, int pagenum, int dirty)
{
	return -1;
}

/*
This function changes the name of oldfile to newfile in both the PF file table and the Unix file system. Obviously, if the oldfile does not exist or the newfile does exist, an error should be returned. This function returns PFE_OK if the operation is successful, an error condition otherwise.

char *oldfile		- an old file name.
char *newfile		- a new file name.
 */
int pf_rename_file(char *oldfile, char *newfile)
{
	return -1;
}

/*
This function writes the string s onto a stream set by pf_set_err_stream() and then write the last error message from the PF onto the stream. No return value.

char *s 			- a string to be printed along with an error message.
 */
void pf_print_error(char *s)
{

}

/*
This function sets the stream used by pf_print_error to fd. No return value.

FILE *fp 			- file pointer.
 */
void pf_set_err_stream(FILE *fp)
{

}




