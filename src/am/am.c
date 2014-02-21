

/* This function initializes the data structures that you will be implementing for the AM layer. This function takes no parameters and produces no return value.
 */
void am_init(void)
{

}

/*
This function creates an index numbered indexno on the file filename. The indexno parameter will have a unique value for each index created on a file. Thus, it can be used with along with filename to generate a unique name for the PF layer file used to implement the index. The type and length of the attribute being indexed are described by the third and fourth parameters. The fifth parameter isunique must be set to FALSE (or can be ignored) because we do not enforce primary key integrity. The job of this function is to create an empty index by creating a PF layer file and initializing it appropriately (i.e. to represent the empty tree). You may assume that all of the indices for a given HF layer file will be created before any records are inserted into the file. It returns AME_OK if it succeeds, and an AM error code otherwise.

char *filename		- name of indexed file
int indexno			- number of this index for file
char attrtype		- 'c', 'i', or 'f'
int attrlength		- 4 for 'i' or 'f', 1-255 for 'c'
bool_t isunique		- uniqueness of key values
 */
int am_create_index(char *filename, int indexno, char attrtype, int attrlength, bool_t isunique)
{
	return -1;
}

/*
This function destroys the index numbered indexno of the file filename by deleting the file that is used to represent it. It returns AME_OK if it succeeds, and an AM error code otherwise.

char *filename		- file name of the base table
int indexno			- index number
 */
int am_destroy_index(char *filename, int indexno)
{
	return -1;
}

/*
This function opens the index numbered indexno of the file filename by calling pf_open_file(). Its return value is the file descriptor for the index (a small positive integer) if the new file has been successfully opened; an AM error code is returned otherwise. You should maintain a table for all open files (indices) of the AM layer (similar to the one used for open files in the HF layer). The file descriptor returned by am_open_index can serve as an index into that table, and you may assume again that no more than MAXOPENFILES = 20 indices will be opened at one time.

char *filename		- file name of the base table
int indexno			- index number
 */
int am_open_index(char *filename, int indexno)
{
	return -1;
}

/*
This function closes the index file with the indicated file descriptor by calling pf_close_file(). It deletes the entry for this file in the table of open AM layer files. It returns AME_OK if the file is successfully closed, and an AM error code otherwise.

int AM_fd 			- file descriptor
 */
int am_close_index(int AM_fd)
{
	return -1;
}

/*
This function a (value, recid) pair into the index represented by the open file associated with AM_fd. The value parameter points to the value to be inserted into the index, and the recid parameter identifies a record with that value to be added to the index. It returns AME_OK if it succeeds , and an AM error code otherwise.

int AM_fd 			- file descriptor
char *value 		- attribute value for the delete
RECID recid 		- id of the record to delete
 */
int am_insert_entry(int AM_fd, char *value, RECID recid)
{
	return -1;
}

/*
This function removes a (value, recid) pair from the index represented by the open file associated with AM_fd. It returns AME_OK if it succeeds, and an AM error code otherwise.

int AM_fd 			- file descriptor
char *value 		- attribute value for the delete
RECID recid 		- id of the record to delete
 */
int am_delete_entry(int AM_fd, char *value, RECID recid)
{
	return -1;
}

/*
This function opens an index scan over the index represented by the file associated with AM_fd. The scan will return the record ids of those records whose indexed attribute value compares in the desired way with the value parameter. The desired comparison will be specified as it is for the hf_open_file_scan() function. The (non-negative) scan descriptor returned is an index into an in-progress index scan table (similar to the one used to implement file scans in the HF layer). You may also assume that no more than MAXSCANS = 20 scans will ever need to be performed at one time. If the index scan table is full, an AM error code is returned in place of a scan descriptor. The op parameter can assume the follow the *_OP symbol values defined in am.h.

int AM_fd 			- file descriptor
int op 				- operator for comparison
char *value 		- value for comparison
 */
int am_open_index_scan(int AM_fd, int op, char *value)
{
	return -1;
}

/*
This function returns the record id of the next record that satisfies the conditions specified for index scan associated with scandesc. If there are no more records satisfying the scan predicate, then an invalid RECID is returned and the AMerrno variable is set to AME_EOF. Other types of errors are returned in the same way.

int scandesc 		- scan descriptor of an index
 */
RECID am_find_next_entry(int scandesc)
{
	return NULL;
}

/*
This function terminates an index scan and disposes of the scan state information. It returns AME_OK if the file is successfully closed, and an AM error code otherwise.

int scandesc 		- scan descriptor of an index
 */
int am_close_index_scan(int scandesc)
{
	return -1;
}

/*
This function writes the string errstring onto stderr, and then writes the last error message produced by the AM layer onto stderr as well. This function will make use of a global integer value, AMerrno, in order to keep track of the most recent error; this error code should be appropriately set in each of the other AM layer function. This function has no return value of its own.

char *errstring 	- pointer to an error message
 */
void am_print_error(char *errstring)
{
	
}