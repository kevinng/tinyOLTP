








/*
This function is to be used to initialize the data structure that you will be maintaining for the HF layer. You may use this as an opportunity to call pf_init(), and to do any initializations needed by the HF layer (for instance, initializing all file table entries to 'not in use'). This function takes no parameters and produces no return value.
 */
void hf_init(void)
{

}

/*
This function calls pf_create_file to create a paged file called filename whose records are of size recsize. In addition, it initializes the file, by storing appropriate information in the header page. It returns HFE_OK if the new file is successfully created, and an HF error code otherwise.

char *filename		- name of the file to be created
int recsize			- record size in bytes
  */
int hf_create_file(char *filename, int recsize)
{
	return -1;
}

/*
This function destroys the file whose name is filename by calling pf_destroy_file(). It is included here only for completeness of the HF layer interface. This function returns HFE_OK if the file is successfully destroyed, and HFE_PF otherwise.

char *filename		- name of the file to be destroyed
 */
int hf_destroy_file(char *filename)
{
	return -1;
}

/*
This function opens a file called filename by calling pf_open_file(). Its return value is the file descriptor for the file (a small positive integer) if the new file has been successfully opened or a HF error code otherwise. In addition to opening the file, this function will make an entry in the table of open HF layer files.

char *filename		- name of the file to be opened
 */
int hf_open_file(char *filename)
{
	return -1;
}

/*
This function closes the file with the file descriptor filedesc by calling pf_close_file(). If successful, it deletes the entry of this file from the table of open HF files. If there is an active scan, however, this function fails and no further action is done. It returns HFE_OK if the file has been successfully closed, HFE_SCANOPEN if there is an active scan, an HF error code otherwise.

int filedesc		- descriptor of the file to be closed
 */
int hf_close_file(int filedesc)
{
	return -1;
}

/*
This function inserts the record pointed to by record into the open file associated with filedesc. This function returns the record id (of type RECID) that was assigned to the newly inserted record if the insertion is successfully, and an HF error code (of the same type) otherwise. Note that error codes are returned by use of a global variable HFerrno, which you need to declare. The RECID returned in case of error is the one which reflects an error condition (e.g. both pagenum and recnum are equal to -1).

int filedesc		- descriptor of the file to be closed
char *record 		- record to be inserted
 */
RECID hf_insert_rec(int filedesc, char *record)
{
	return NULL;
}

/*
This function deletes the record indicated by recid from the file associated with filedesc. It returns HFE_OK if the deletion is successful, and an HF error code otherwise.

int filedesc		- file descriptor of an open file
RECID recid 		- id of the file to be deleted
 */
int hf_delete_rec(int filedesc, RECID recid)
{
	return -1;
}

/*
This function retrieves a copy of the first record in the file associated with filedesc. If it succeeds, the record id of the first record in the file is returned, and a copy of the record itself is placed in the location pointed by record. If the file is empty, it returns HFE_EOF, otherwise it returns an HF error code. Note that error codes are returned by the use of a global variable HFerrno, which you need to declare. In the case of error, the RECID returned is one which reflects an error condition (e.g. record number equals to -1).

int filedesc 		- file descriptor of an open file
char *record 		- pointer to the record buffer
 */
RECID hf_get_first_rec(int filedesc, char *record)
{
	return NULL;
}

/*
This function retrieves a copy of the record following the one with id recid in the file associated with filedesc. If it succeeds, the record id of this record is returned, and a copy of the record is placed in the location pointed to by record. If there are no more records in the file, it returns HFE_EOF. If the incoming record id is invalid, it returns HFE_INVALIDRECORD, otherwise it returns another HF error code. Error codes are returned by the use of a global variable HFerrno, which you need to declare. The RECID returned in case of error, is one which reflects an error condition (e.g. record number equals to -1).

int filedesc		- file descriptor of an open file
RECID recid 		- record id whose next one will be retrieved
char *record 		- pointer to the record buffer
 */
RECID hf_get_next_rec(int filedesc, RECID recid, char *record)
{
	return NULL;
}

/*
This function retrieves a copy of the record with recid from the file associated with filedesc. The data is placed in the buffer pointed to by record. It returns HFE_OK if it succeeds, HFE_INVALIDRECORD if the argument recid is invalid, HFE_EOF if the record recid does not exist, or another HF error code otherwise.

int filedesc		- file descriptor of an open file
RECID recid 		- id of the record that will be retrieved
char *record 		- pointer to the record buffer
 */
int hf_get_this_rec(int filedesc, RECID recid, char *record)
{
	return -1;
}

/*
This function opens a scan over the records in the file associated with filedesc whose value for the indicated attribute satisfies the specified condition. The attrtype field represents the attribute type, which can be character/string (c), integer (i), or float (f). If value is a null pointer, then a scan of the entire file is desired. Otherwise, value will point to the (binary) value that records are to be compared with. The (non-negative) scan descriptor returned is an index into a table (implemented and maintained by your HF layer code) used for keeping track of information about the state of in-progress file scans. Information such as the record id of the record that was just scanned, what files are open due to the scan, etc, are to be kept in this table. (You should design the contents of this table based on what you find yourself needing to know.) You may assume that no more than MAXSCANS = 20 scans will ever need to be performed at one time. If the scan table is full, a HF error code is returned in place of a scan descriptor. The parameter op determines the way that the value parameter is compared to the record's indicated attribute value. The different comparison options are encoded in op as follows:

	1 for EQUAL (i.e. attribute = value)
	2 for LESS THAN (i.e. attribute < value)
	3 for GREATER THAN (i.e. attribute > value)
	4 for LESS THAN OR EQUAL (i.e. attribute <= value)
	5 for GREATER THAN OR EQUAL (i.e. attribute >= value)
	6 for NOT EQUAL (i.e. attribute != value)

int filedesc			- file descriptor
char attrtype			- 'c', 'i' or 'f'
int attrlength			- 4 for 'i' or 'f', 1-255 for 'c'
int attroffset			- offset of attribute for comparison
int op 					- operator for comparison
char *value 			- value for comparison (or null)
 */
int hf_open_file_scan(int filedesc, char attrtype, int attrlength, int attroffset, int op, char *value)
{
	return -1;
}

/*
This function retrieves a copy of the next record in the file being scanned through scandesc that satisfies the scan predicate. If it succeeds, it returns te record id of this record and places a copy of the record in the location pointed to by record. It returns HFE_EOF if there are no records left to scan in the file, and an HF error code otherwise. Error codes are returned by use of a global variable HFerrno, which you need to declare. The RECID returned in case of error, is one which reflects an error condition (e.g. record number equals to -1).

int scandesc			- description of the file being scanned
char *record 			- pointer to the record buffer
  */
RECID hf_find_next_rec(int scandesc, char *record)
{
	return NULL;
}

/*
This function terminates the file scan indicated by scandesc. It returns HFE_OK if it succeeds, and an HF error code otherwise.

int scandesc 			- descriptor of the file being scanned
 */
int hf_close_file_scan(int scandesc)
{
	return -1;
}

/*
This function writes the string errstring onto stderr, and then writes the last error message produced by the HF layer onto stderr as well. This function has no return value of its own. This function is just a switch(HFerrno) statement where HFerrno represents the last HF layer error occured.

char *errstring 		- pointer to an error message
 */
void hf_print_error(char *errstring)
{

}

/*
This function returns TRUE if the recid is a valid one, and FALSE otherwise. TRUE and FALSE are defined in the tinyrel.h file. The notion of recid validity is based on the value ranges of its pagenum and recnum. This function will return TRUE as long as both the values are in proper ranges for a given HF file. In other words, a valid recid can point to a record slot which may be occupied by an existing record or may be empty.

int filedesc 			- file descriptor of an open file
RECID recid 			- id of the record to be validated
 */
bool_t hf_valid_rec_id(int filedesc, RECID recid)
{
	return NULL;
}