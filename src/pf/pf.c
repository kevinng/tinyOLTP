#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/file.h>
#include <unistd.h>
#include <stdlib.h>
#include "../test/dbg.h"
#include "pf.h"
#include "tinyoltp.h"

/* Output Settings. */
static FILE *err_stream = NULL; /* Error messages are printed to this file. */
#define TEMP_ROOT	"temp/"	 	/* Root folder for all temporary outputs. */

/* Paged file table. */
static PFftab_ele *pf_file_table;		/* The PF file table. */
static unsigned int pf_file_count;	 	/* The number of elements in the 
										PF file table. */

/* Buffer manager. */
static PFbpage *PFfirstbpage = NULL; 		/* Ptr to first buffer page,
											or NULL */
static PFbpage *PFlastbpage = NULL; 		/* Ptr to last buffer page, 
											or NULL */
static PFbpage *PFfreebpage = NULL; 		/* List of free buffer page */
static PFhash_entry *pf_hash_table = NULL;	/* The hash table to access a 
											buffer page. */
static unsigned int pf_hash_count;			/* Number of entries in the 
											hash table. */

/* Last error message. */
#define ERR_MSG_MAX_LEN	256
static char last_err_msg[ERR_MSG_MAX_LEN];

/**
 * Initializes the PF layer. Must be called before using the PF layer.
 */
void pf_init()
{
	free(pf_file_table);
	free(pf_hash_table);
	free(PFfreebpage);

	PFfirstbpage = NULL;
	PFlastbpage = NULL;

	pf_file_count = 0;
	pf_hash_count = 0;

	pf_file_table = malloc(PF_FTAB_SIZE * sizeof(PFftab_ele));

	/* Allocate memory for the buffer hash table. */
	pf_hash_table = malloc(PF_HASH_TBL_SIZE * sizeof(PFhash_entry));

	/* Allocate memory for the buffer list - note: for the buffer structures, NOT the page data (the buffer refers to the page data via a pointer to PFfpage structure). */
	PFfreebpage = malloc(PF_MAX_BUFS * sizeof(PFbpage));
	/* Create paged file structures for all buffer pages - without allocating
	memory for the page data. */
	int i;
	for (i = 0; i < PF_MAX_BUFS; i++) {
		PFfpage new_pffp;
		PFBpage *pfbp = PFfreebpage + i;
		pfbp = &new_pffp;
	}
	
	/* Set the error stream. */
	FILE *err_fp;
	if ((err_fp = fopen(TEMP_ROOT "error.log", "a")) == NULL)
		exit(1);
	pf_set_err_stream(err_fp);

	/* Null-terminate the last error message. */
	last_err_msg[0] = '\0';
}

/**
 * Creates a new PF layer file.
 * 
 * @param  filename name of the file to be created.
 * @return          PFE_OK if successful, Returns an error code on error.
 */
int pf_create_file(char *filename)
{
	int fd, perms = 0666;

	/* Tests if file filename exists by opening it. */
	if (open(filename, O_RDONLY, perms) != -1)
		return -1;

	/* Create a new file. */
	if ((fd = creat(filename, perms)) == -1)
		return -1;

	/* The header - the first free page in the file followed by the number of pages in the file. */
	if (pf_write_hdr(fd, 0, 0) == PFE_OK)
		return PFE_OK;
	else {
		/* Undo what we did, and delete the (incomplete) file we've 
		created. */
		unlink(filename);
	}

	return -1;
}

/**
 * Destroy the file filename. If the file is open, it will be closed by 
 * calling pf_close_file().
 * 
 * @param  filename name of the file to be destroyed.
 * @return          PFE_OK if successful. -1 on error.
 */
int pf_destroy_file(char *filename)
{
	/* Find the file descriptor of the filename. */
	int fd = -1;
	int i;
	for (i = 0; i < (int)pf_file_count; i++) {
		if (strcmp((pf_file_table+i)->fname, filename) == 0) {
			fd = i;
			break;
		}
	}

	if (fd != -1) {
		if (pf_close_file(fd) != PFE_OK)
			return -1;
	}

	if (unlink(filename) != -1)
		return -1;

	return PFE_OK;
}

/**
 * Open PF layer file.
 * 
 * @param  filename name of the file to be opened.
 * @return          PF file descriptor of the newly opened file. -1 if an 
 * error occured.
 */
int pf_open_file(char *filename)
{
	/* Make sure the PF file table is not full. */
	if (pf_file_count >= PF_FTAB_SIZE)
		return -1;
	
	/* Open file. */
	int perms = 0666;
	int fd = open(filename, O_RDWR, perms);
	if (fd == -1)
		return -1;

	/* Obtain the inode of the file. */
	struct stat stbuf;
	int fstat_ret_code = fstat(fd, &stbuf);
	debug("fstat_ret_code: %d", fstat_ret_code);
	if (fstat_ret_code == -1)
		return -1;

	/* Make sure this file has not been opened before. */
	int pff_cnt = pf_file_count;
	PFftab_ele *pf_file_ele;
	while (pff_cnt-- > 0) {
		pf_file_ele = pf_file_table+pff_cnt;
		/* Find a PFftab_ele with the same name and inode number as the one
		just opened. */
		if (pf_file_ele->inode == stbuf.st_ino && 
			strcmp(pf_file_ele->fname, filename) == 0)
			return -1;
	}

	/* Initialize new PFftab_ele. */
	struct PFftab_ele ftab_ele;
	ftab_ele.valid = TRUE;
	ftab_ele.inode = stbuf.st_ino;
	ftab_ele.fname = malloc(strlen(filename)+1);
	strcpy(ftab_ele.fname, filename);
	ftab_ele.unixfd = fd;
	read(fd, &(ftab_ele.hdr.firstfree), sizeof(ftab_ele.hdr.firstfree));
	read(fd, &(ftab_ele.hdr.numpages), sizeof(ftab_ele.hdr.numpages));
	ftab_ele.hdrchanged = FALSE;

	/* Assign ftab_ele to the file table. */
	/* Note: memory has already been allocated at initialization. */
	*(pf_file_table+pf_file_count) = ftab_ele;
	pf_file_count++;

	return pf_file_count;
}

/**
 * Closes the file associated with the PF file descriptor fd. The buffers of 
 * the file are flushed (meaning the dirty pages are written back). If the 
 * file header has changed, it is written back to the file. The file is 
 * finally closed by using the system call close(). The file table entry 
 * corresponding to that file is made invalid.
 * 
 * @param  fd paged file descriptor of the file to close.
 * @return    PFE_OK if successful, -1 otherwise.
 */
int pf_close_file(int fd)
{
	/* Write dirty pages back to file (if dirty). */

	/* Iterate the double linked list. */
	PFbpage *this_bpage = PFfirstbpage;
	while (this_bpage != PFlastbpage) {
		if (this_bpage->fd == fd &&
			this_bpage->dirty == TRUE) {

			if (this_bpage->count > 0)
				return -1; /* Page is pinned. 
					Flushing it might cause inconsistency. */

			/* Write page. */
			pf_write_page(fd,
				this_bpage->fpage->nextfree,
				this_bpage->pagenum,
				this_bpage->fpage->pagebuf);
		}

		this_bpage++;
	}

	/* Index the paged file table element. */
	PFftab_ele *pfft_ele = pf_file_table + fd;

	/* Write file header back to file (if dirty). */
	if (pfft_ele->hdrchanged)
		pf_write_hdr(fd, pfft_ele->hdr.firstfree, pfft_ele->hdr.numpages);

	/* Close Unix file. */
	close(pfft_ele->unixfd);

	/* Move all entries backwards. */
	PFftab_ele *last_ele = pf_file_table + pf_file_count;
	PFftab_ele *next_ele;
	while (pfft_ele != last_ele) {
		next_ele = pfft_ele + 1;
		pfft_ele = next_ele;
		pfft_ele++;
	}
	
	return PFE_OK;
}

/**
 * Gets the first valid page after page pagenum in file fd.
 * 
 * @param  fd      PF file descriptor.
 * @param  pagenum set to the starting page number. Returns page number of the
 *                 first page after the starting page number.
 * @param  pagebuf return data of the page.
 * @return         PFE_OK if successful. FE_EOF if the end of file is reached
 *                        without finding any used page data, PFE_INVALIDPAGE
 *                        if page is invalid, a PF error code otherwise.
 */
int pf_get_next_page(int fd, int *pagenum, char *pagebuf)
{
	/* WRONG - I NEED TO GET IT UP INTO MEMORY FIRST! */

	*pagenum = -1;
	int i;
	for (i = 1; /* Start from the page after pagenum. */
		read((pf_file_table+fd)->unixfd, 
			&pagenum, 
			sizeof(int)) == sizeof(int);
		i++) {
		if (lseek((pf_file_table+fd)->unixfd, 
			pf_pagenum_to_file_pos(*pagenum + i), 
			0) != -1)
			return PFE_INVALIDPAGE;

		if (*pagenum == -1)
			break;
	}

	if (*pagenum == -1)
		return PFE_INVALIDPAGE;

	/* Continue fetching this page. */
	pagebuf = malloc(PF_PAGE_SIZE);
	if (read((pf_file_table+fd)->unixfd, 
		pagebuf, PF_PAGE_SIZE) != PF_PAGE_SIZE) {
		free(pagebuf);
		return PFE_INVALIDPAGE;
	}

	return PFE_OK;
	
	/*
	
	how do i want to test this?


	create a empty file
	try access with -1, it should return the first page.

	create a file with 3 entries, both with data.
	try accessing with pagenum1, it should return pagenum 2

	create a file with 3 entries, 2nd with no data
	try access with page num 1, it should return pagenum 3

	create a file with 3 entries, 2nd free
	try accessing with pagenum 2, it should return 3

	create a file with 3 entries, all with data,
	try accessing with pagenum 1, it should return 2

	create a file with 3 entries, all with data,
	try accessing with pagenum 2, it should return 3

	create a file with 3 entries, 3 with no data
	try acessing with 2, it should return EOF

	create a file with 3 entries, all with data
	try accessing with 4, it should return invalid pagenum

	create a file with 3 entries, all with data
	try accessing with -2, it should return invalid pagenum
	 */
}

/**
 * Returns first valid page in the file associated with file descriptor fd.
 * 
 * @param  fd      PF file descriptor.
 * @param  pagenum return page number of the first page.
 * @param  pagebuf return data of the page.
 * @return         PFE_OK if successful. Returns the error codes 
 * as pf_get_next_page() otherwise.
 */
int pf_get_first_page(int fd, int *pagenum, char *pagebuf)
{
	return pf_get_next_page(fd, pagenum, pagebuf);
}

/**
 * Reads a valid page pagenum from the file fd, and sets *pagebuf to point
 * to the page data. The page is then pinned in memory. An invalid page is
 * not pinned in memory.
 * 
 * @param  fd      PF file descriptor.
 * @param  pagenum page number of the page to retrieve.
 * @param  pagebuf return data of the page.
 * @return         PFE_OK if successful. -1 otherwise.
 */
int pf_get_this_page(int fd, int pagenum, char *pagebuf)
{
	/* Check if the page exists in memory. */
	PFbpage *bpage = pf_bpage_lookup(fd, pagenum);
	if (bpage) {
		/* Access from buffer. */
		pagebuf = bpage->fpage->pagebuf;
		return PFE_OK;
	}
	
	/* Get a free buffer page. */
	bpage = pf_get_free_buffer_page();
	if (!bpage)
		return -1; /* No buffer page can be allocated. */

	if (lseek((pf_file_table+fd)->unixfd,
		pf_pagenum_to_file_pos(pagenum),
		0) == -1)
		return -1;

	int nextfree;
	if (read((pf_file_table+fd)->unixfd,
		&nextfree,
		sizeof(nextfree)) != sizeof(nextfree))
		return PFE_INVALIDPAGE;

	if (nextfree != -1)
		return PFE_INVALIDPAGE;

	pagebuf = malloc(PF_PAGE_SIZE);
	if (read(pf(pf_file_table+fd)->unixfd,
		pagebuf,
		PF_PAGE_SIZE) != PF_PAGE_SIZE) {
		free(pagebuf);
		return PFE_INVALIDPAGE;
	}

	/* Initialize new buffer page settings. */
	bpage->count = 1; /* Pin this page. */
	bpage->pagenum = pagenum;
	bpage->fd = fd;
	bpage->fpage->pagebuf = pagebuf;

	return PFE_OK;
}

/**
 * Allocates a page in file fd. If there is a free page in the file, the free
 * page is reused without adding a new page to the file. Otherwise, a new valid
 * page is added to the file.
 * 
 * @param  fd      [description]
 * @param  pagenum return page number of the page allocated.
 * @param  pagebuf return data of the page.
 * @return         PFE_OK if successful. -1 otherwise.
 */
int pf_alloc_page(int fd, int *pagenum, char *pagebuf)
{
	/* Index the paged file table element. */
	PFftab_ele *pfft_ele = pf_file_table + fd;

	/* The page number is this free page we are giving out. */
	*pagenum = pfft_ele->hdr.firstfree;
	
	/* Get a free buffer page. */ 
	PFbpage *buffer_page = pf_get_free_buffer_page();
	if (!buffer_page)
		return -1; /* Unable to allocate buffer page. */

	/* Update the list tracking the list of free pages. */
	/* Check if we are at the end of file. */
	if (pfft_ele->hdr.firstfree >= pfft_ele->hdr.numpages) {
		/* If so, the nextfree position is simply the page after this one.
		Simply increment the header's firstfree value. */
		pfft_ele->hdr.firstfree += 1;
		buffer_page->fpage->nextfree = pfft_ele->hdr.firstfree;
	} else {
		/* Move to the start of the page pagenum. */
		if (lseek(pfft_ele->unixfd, pf_pagenum_to_file_pos(*pagenum), 0)
			== -1)
			return -1;
		/* Set the firstfree value in the header with the nextfree value in
		this free page's header. */
		int nextfree;
		if (read(pfft_ele->unixfd, &nextfree, sizeof(nextfree)) 
			!= sizeof(nextfree))
			return -1; /* Quit to prevent corrupting the file header. */

		pfft_ele->hdr.firstfree = nextfree;
	}

	/* Allocate memory for the paged file. */
	buffer_page->fpage = malloc(PF_PAGE_SIZE);

	/* Point pagebuf to the newly initialized paged file structures' 
	page data. */
	pagebuf = buffer_page->fpage->pagebuf;
	
	/* Initialize pagenum and fd. */
	buffer_page->count = 1; /* Pin this page. */
	buffer_page->pagenum = *pagenum;
	buffer_page->fd = fd;

	/* Initialize new hash table entry for the new buffer page. */
	PFhash_entry new_he;
	new_he.fd = fd;
	new_he.pagenum = *pagenum;
	new_he.bpage = buffer_page;
	
	/* Insert the new hash table entry into the hash table. */
	unsigned hash_idx = pf_hash(fd, *pagenum);
	PFhash_entry *old_he = pf_hash_table+hash_idx;

	if (!old_he) {
		old_he->preventry = &new_he;
		new_he.preventry = NULL; /* The new hash entry is now the first 								entry in the bucket. */
	}

	old_he = &new_he; 	/* Let the new hash entry take the index position 
						in the hash table for this bucket. */

	return PFE_OK;
}

/**
 * Disposes (i.e. free) page pagenum in file fd.
 * 
 * @param  fd      PF file descriptor fd.
 * @param  pagenum page number of the page to dispose.
 * @return         PFE_OK if successful. -1 otherwise.
 */
int pf_dispose_page(int fd, int pagenum)
{
	/* Lookup the hash entry in the table. */
	PFhash_entry *hash_entry = pf_hash_lookup(fd, pagenum);
	if (!hash_entry) {
		/* Page is not in memory - bring it into memory. */
		char *pagebuf = NULL;
		if (pf_get_this_page(fd, pagenum, pagebuf) == -1)
			return -1;

		/* Lookup the hash entry again. */
		hash_entry = pf_hash_lookup(fd, pagenum);
	}

	/* Check if the page has been fully unpinned. */
	if (hash_entry->bpage->count > 0)
		return -1;

	/* Free up the memory allocated. */
	free(hash_entry->bpage->fpage->pagebuf);

	/* Set this page's nextfree value to the header's first free value. */
	hash_entry->bpage->fpage->nextfree = (pf_file_table+fd)->hdr.firstfree;

	/* Set the header's firstfree value to this page. */
	(pf_file_table+fd)->hdr.firstfree = pagenum;
	(pf_file_table+fd)->hdrchanged = TRUE; /* Mark the 
										file header as dirty. */

	return PFE_OK;
}

/**
 * Marks the pagenum of the file referenced by fd dirty.
 * 
 * @param  fd      PF file descriptor
 * @param  pagenum page number of the page to be marked dirty
 * @return         PFE_OK if successful. -1 otherwise.
 */
int pf_dirty_page(int fd, int pagenum)
{
	/* Lookup the hash entry in the table. */
	PFhash_entry *hash_entry = pf_hash_lookup(fd, pagenum);
	if (!hash_entry)
		return -1; /* Invalid fd and pagenum. */
	
	hash_entry->bpage->dirty = TRUE;
	
	return PFE_OK;
}

/**
 * After checking the validity of the fd and pagenum values, this function
 * unpins the file numbered pagenum of the file with the file descriptor fd.
 * 
 * @param  fd      PF file descriptor.
 * @param  pagenum page number of the page to be unpinned.
 * @return         PFE_OK if successful. -1 otherwise.
 */
int pf_unfix_page(int fd, int pagenum)
{
	/* Lookup the hash entry in the table. */
	PFhash_entry *hash_entry = pf_hash_lookup(fd, pagenum);
	if (!hash_entry)
		return -1; /* Invalid fd and pagenum. */

	hash_entry->bpage->dirty = TRUE; /* Mark as dirty. */
	
	if (hash_entry->bpage->count > 0)
		hash_entry->bpage->count--; /* Unpin the page. */ 
	
	return PFE_OK;
}

/**
 * If the dirty parameter if TRUE, then it first marks the page dirty before
 * it is unpinned. Otherwise, the effect of this function is identical to 
 * that of pf_unfix_page().
 * 
 * @param  fd      PF file descriptor.
 * @param  pagenum page number of the page to be unpinned.
 * @param  dirty   dirty indication.
 * @return         PFE_OK if successful. -1 otherwise.
 */
int pf_unpin_page(int fd, int pagenum, int dirty)
{
	/* Lookup the hash entry in the table. */
	PFhash_entry *hash_entry = pf_hash_lookup(fd, pagenum);
	if (!hash_entry)
		return -1; /* Invalid fd and pagenum. */

	if (dirty)
		hash_entry->bpage->dirty = TRUE; /* Mark as dirty. */

	if (hash_entry->bpage->count > 0)
		hash_entry->bpage->count--; /* Unpin the page. */

	return PFE_OK;
}

/**
 * Renames paged file old_file to new_file.
 * 
 * @param  old_file the old filename.
 * @param  new_file the new filename.
 * @return         	PFE_OK if successful. -1 otherwise.
 */
int pf_rename_file(char *old_file, char *new_file)
{
	/* Check if both files exists. */
	int perms = 0666;
	if (open(old_file, O_RDONLY, perms) == -1 ||
		open(new_file, O_RDONLY, perms) == -1)
		return -1;
	
	/* Find the PF table element with the filename. */
	unsigned i;
	int fd = -1;
	PFftab_ele *pfftab_ele;
	for (i = 0; i < pf_file_count; i++) {
		pfftab_ele = pf_file_table + i;
		if (strcmp(pfftab_ele->fname, old_file) == 0) {
			fd = i;
			break;
		}
	}

	if (fd == -1)
		return -1; /* Can't find the old file in the paged file table. */
	
	/* Close the old file. */
	if (pf_close_file(fd) != PFE_OK)
		return -1;

	/* Rename the file. */
	if (rename(old_file, new_file) != 0)
		return -1;

	return PFE_OK;
}

/**
 * Write string s onto the stream set by pf_set_err_stream(), and write the
 * last error message from the PF onto the stream.
 * 
 * @param s string to be printed along with an error message.
 */
void pf_print_error(char *s)
{
	fprintf(err_stream, "%s: %s", s, last_err_msg);
}

/**
 * Sets error stream used by pf_print_error() to fp.
 * 
 * @param fp error stream to be used by pf_print_error();
 */
void pf_set_err_stream(FILE *fp)
{
	err_stream = fp;
}

long pf_pagenum_to_file_pos(int pagenum)
{
	size_t hdr_size = 2 * sizeof(int);
	size_t page_size = (2*sizeof(int)) + PF_PAGE_SIZE; 
		/* Page header + page size */
	return hdr_size + page_size * pagenum;
}

int pf_write_hdr(int fd, int firstfree, int numpages)
{
	/* File header data to be written. */
	int hdr[] = {firstfree, numpages}; 
	
	/* Seek to the beginning to the file. */
	if (lseek(fd, 0L, 0) == -1)
		return -1;
	int n_wrote = write(fd, hdr, sizeof(hdr));

	/* Make sure we've wrote the right number of bytes to the file. */
	if (n_wrote != sizeof(hdr))
		return -1;
	
	/* Close the file. */
	close(fd);
	
	return PFE_OK;
}

int pf_write_page(int fd, int nextfree, int pagenum, char *pagebuf)
{
	/* Index the page file table, and get the Unix file descriptor of the
	paged file. */
	int unix_fd = (pf_file_table+fd)->unixfd;

	/* Physical file position of the page we want to overwrite. */
	int file_pos = pf_pagenum_to_file_pos(pagenum);

	/* Seek the page position in the file. */
	if (lseek(unix_fd, file_pos, 0) == -1)
		return -1;
	/* Note: it is legal to write beyond the end of the file. The gap will be filled in with zeros. */

	/* Write page header. */
	int page_hdr[2] = {nextfree, pagenum};
	if (write(unix_fd, page_hdr, sizeof(page_hdr)) == sizeof(page_hdr))
		return PFE_OK;

	/* Write page data. */
	if (write(unix_fd, pagebuf, PF_PAGE_SIZE) == PF_PAGE_SIZE)
		return PFE_OK;

	return -1;
}

unsigned pf_hash(int fd, int pagenum)
{
	unsigned hashval;
	char hash_str[3] = {fd, pagenum, '\0'};

	int i;
	for (hashval = 0, i = 0; hash_str[i] != '\0'; i++)
		hashval = *hash_str + 31 * hashval;

	return hashval % PF_HASH_TBL_SIZE;
}

PFhash_entry *pf_hash_lookup(int fd, int pagenum)
{
	/* Index the hash entry for fd/pagenum to access the hash bucket. */
	PFhash_entry *hash_entry = pf_hash_table + pf_hash(fd, pagenum);

	/* Look for the hash entry in bucket with the matching fd and pagenum. */
	while (hash_entry != NULL) {
		if (hash_entry->fd == fd && hash_entry->pagenum == pagenum)
			return hash_entry; /* Page found. */
		hash_entry = hash_entry->nextentry;
	}

	return NULL;
}

PFbpage *pf_bpage_lookup(int fd, int pagenum)
{
	/* Index the hash table. */
	PFhash_entry *pfhe = pf_hash_lookup(fd, pagenum);
	if (!pfhe)
		return NULL;

	/* Scan the bucket for the right buffer entry. */
	this_pfhe = pfhe;
	while (this_pfhe != NULL) {
		if (this_pfhe->bpage.fd == fd &&
			this_pfhe->bpage.pagenum == pagenum)
			return this_pfhe->bpage;
		this_pfhe = pfhe->nextentry;
	}

	return NULL;
}

int pf_get_fd_of_filename(char *filename)
{
	unsigned i;
	for (i = 0; i < pf_file_count; i++) {
		PFftab_ele *pfft_ele = (pf_file_table + i);
		if (strcmp(pfft_ele->fname, filename) == 0)
			return i;
	}

	return -1;
}

PFbpage *pf_get_free_buffer_page()
{
	PFbpage = buffer_page;

	/* Check the free list of buffer pages. */
	if (!PFfreebpage) {
		/* Use one of the free buffer page available. */
		buffer_page = PFfreebpage++;

		/* Allocate the new page and put it at the beginning of double linked list to support the LRU page replacement strategy. */
		buffer_page->nextpage = PFfirstbpage;
		buffer_page->prevpage = NULL;
		PFfirstbpage = buffer_page;
	}
	else {
		/* All the buffer pages has been allocated during init.

		If there is no buffer page in the free list, then we have already hit reached the maximum number of allowable buffer page, and we must perform page replacement (according to the LRU policy). */

		/* Go backwards on the list of buffer pages, and find one that's not pinned. If it is dirty, write it back onto the disk. Then free the page and give it to here. */
		PFbpage *bp_to_free = PFlastbpage;	
		while (bp_to_free != PFfirstbpage) {
			if (!bp_to_free)
				return -1;
			
			if (bp_to_free->count == 0) {
				if (bp_to_free->dirty) {
					/* Write the page back to the disk. */
					if (pf_write_page(bp_to_free->fd,
						bp_to_free->fpage->nextfree,
						bp_to_free->pagenum,
						bp_to_free->fpage->pagebuf) != PFE_OK)
						return -1;
				}

				buffer_page = bp_to_free;
				/* Note: the buffer page can stay at its current position in the double linked list. There is no need to update its position. */
				break;
			}

			bp_to_free++;
		}
	}

	/* Set the buffer page to default values. */
	buffer_page->dirty = FALSE;
	buffer_page->count = 0;
	buffer_page->pagenum = -1;
	buffer_page->fd = -1;
	buffer_page->fpage->nextfree = -1;

	return buffer_page;
}