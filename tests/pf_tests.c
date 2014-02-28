#include <sys/file.h>
#include <unistd.h>
#include "../src/pf/pf.h"
#include "../src/test/minunit.h"

#define TEST_TEMP_ROOT	"tests/temp/"	 /* Root folder for all temporary
 outputs. */

/* pf_create_file() */

char *test_pf_create_file_should_fail_if_file_exists()
{
	pf_init();
	
	/* File we want to create. */
	char *filename = TEST_TEMP_ROOT "file_that_exists.pf"; 
	int perms = 0666; /* read/write for owner, group and others. */
			
	/* Make sure a file does not exists. */
	int fd = open(filename, O_RDONLY, perms);
	debug("fd: %d", fd);
	mu_assert(fd == -1, "Should not be able to open a file that does not exists.");
	close(fd);

	/* Create the file outself with sample content and make sure it exists. */
	mu_assert((fd = creat(filename, perms)) != -1,
		"Should be able to create file");
	/* Write a sample string onto the file. */
	char *sample_str = "hello world";
	char *str_start = sample_str;
	while (*sample_str++ != '\0') ;
	int str_len = sample_str - str_start;
	int n_wrote = write(fd, str_start, str_len);
	mu_assert_with_clean_up(str_len == n_wrote, "Should be able to write sample string onto the file.", unlink(filename));
	close(fd);

	/* Make sure the file exists. */
	mu_assert((fd = open(filename, O_RDONLY, perms)) != -1, "File should already exists.");
	close(fd);

	/* Call our function and make sure it fails with the right error 
		message. */
	int ret_code = pf_create_file(filename);
	debug("ret_code: %d", ret_code);
	mu_assert(ret_code == -1, "File should already exist, and pf_create_file() should fail.");
	
	/* Open the file and make sure the sample contents are still the same. */
	mu_assert((fd = open(filename, O_RDONLY, perms)) != -1, "Should be able to open file for reading");
	char buf[str_len];
	int n_read = read(fd, buf, str_len);
	debug("n_read: %d", n_read);
	mu_assert(strcmp(buf, str_start) == 0, "The string written to the file should be remain the same.");
	close(fd);

	/* Clean up the file we created. */
	unlink(filename);

	/* Make sure the file is gone. */
	mu_assert(open(filename, O_RDONLY, perms) == -1, "Should not be able to open a file that does not exists.");

	/* Clean up. */
	pf_close_file(fd);
	pf_destroy_file(filename);
	
	return NULL;
}

char *test_pf_create_file_should_work_with_valid_input()
{
	pf_init();
	
	/* File we want to create. */
	char *filename = TEST_TEMP_ROOT "file_to_create.pf";
	int fd, perms = 0666;

	/* Make sure the file does not exists. */
	mu_assert(open(filename, O_RDONLY, perms) == -1, "Should not be able to open a file that does not exists.");

	/* Call our function. */
	pf_create_file(filename);

	/* Read the file and make sure the contents are exactly as expected. */
	fd = open(filename, O_RDONLY, perms);
	int hdr[2] = {-1, -1};
	int n_read = read(fd, hdr, sizeof(hdr));
	mu_assert(n_read == sizeof(hdr),
		"The number of header bytes is not what we expect.");
	mu_assert(hdr[0] == 0, "The first free page is not 0.");
	mu_assert(hdr[1] == 0, "The number of pages is not equal to 0.");
	close(fd);

	/* Clean up. */
	unlink(filename);

	/* Make sure the file is gone. */
	mu_assert(open(filename, O_RDONLY, perms) == -1,
		"Should not be able to open a file that does not exists.");

	/* Clean up. */
	pf_close_file(fd);
	pf_destroy_file(filename);

	return NULL;
}


/* pf_write_page() / pf_get_this_page() */
/*
how do i want to test this?



test if the page is pinned in memory



create a page with nothing to it

write to the 2nd location
read the 2nd location - should work
read the first location - should not work

write to the 1st location
read the first location - should work

clean up

 */

/* pf_dispose_page() */

/*
how do i want to test this?

create a file with nothing - except header of course...

write a page to it
dispose it - getting the page should not work

write 2 pages to it
dispose the first page
get the 2nd page - should work
get the first page - should not work.

clean up

 */

/* pf_get_this_page() */
/*
how do i want to test this?

create a file with nothing - except header of course..

write a page to it
retrieve it - does it work?

write a page after it
retrieve it - does it work?
retrieve the previous page - does it work?

free first page - call dispose page
retrieve te 2nd page, it should work
retrueve the first page, it should not work.

clean up.

 */

/* pf_open_file() */

char *test_pf_open_file_should_fail_if_file_does_not_exists()
{
	pf_init();
	
	char *filename = TEST_TEMP_ROOT "file_that_don't_exists.pf";
	mu_assert(pf_open_file(filename) == -1,
		"Should not be able to open a file that does not exist");

	/* Clean up. */
	// pf_close_file(fd);
	pf_destroy_file(filename);
	
	return NULL;
}

char *test_pf_open_file_should_fail_if_we_open_the_same_file_twice()
{
	pf_init();
	
	/* Create the file we want to open twice. */
	char *filename = TEST_TEMP_ROOT "file_we_want_to_open_twice.pf";
	int ret_code = pf_create_file(filename);
	debug("pf_create_file() return code: %d", ret_code);
	debug("PFE_OK: %d", PFE_OK);
	mu_assert(ret_code == PFE_OK, "Should be able to create the file.");

	/* Open the file the first time - should be okay. */
	ret_code = pf_open_file(filename);
	debug("pf_open_file() return code: %d", ret_code);
	mu_assert(ret_code != -1, "File should open the first time we try.");

	/* Open the file the second time - should fail. */
	ret_code = pf_open_file(filename);
	debug("pf_open_file() return code: %d", ret_code);
	mu_assert(ret_code == -1, "File should not open the second time we try.");

	/* Clean up. */
	unlink(filename);

	/* Make sure the file is gone. */
	int perms = 0666;
	mu_assert(open(filename, O_RDONLY, perms) == -1, 
		"Should not be able to open a file that does not exists.");

	/* Clean up. */
	pf_close_file(pf_get_fd_of_filename(filename));
	pf_destroy_file(filename);

	return NULL;
}

/* pf_write_page() */

/*
how do i want to test this?

i want to create a new empty paged file.

make sure the new paged file is really empty.

write to the page immediately following the header.

read the contents, and find out about it.

make sure it is what we have written to the file.


write one more page to the follow that, and read that page

write one more page two pages after that

and read that page - it should be the same

write the page before that - it should be that page

overwrite the first page - it should work

 */

char *test_write_page()
{
	pf_init();

	/* Create a new PF file. */
	char *filename = TEST_TEMP_ROOT "/file_to_write_to.pf";
	mu_assert(pf_create_file(filename) == PFE_OK,
		"Should be able to create file.");

	/* Open the file. */
	int fd = pf_open_file(filename);

	/* Write a file header to the file. */
	mu_assert(pf_write_hdr(fd, 0, 0) == PFE_OK,
		"Should be able to write header");

	/* Write to the page immediately following the header. */
	char *write_pbuf = malloc(PF_PAGE_SIZE); /* Data we want to write. */
	int i, j;
	for (i = j = 0; i < PF_PAGE_SIZE; i++, j = j == 1 ? 0 : 1)
		*write_pbuf = j;
	/* Write to page 1, setting the nextfree value to 25 (hypothetically) */
	mu_assert(pf_write_page(fd, 25, 1, write_pbuf) == PFE_OK,
		"Should be able to write page to the file.");

	/* Read the contents, and make sure the contents is what 
	we wrote to it. */
	char *read_pbuf = malloc(PF_PAGE_SIZE);
	mu_assert(pf_get_this_page(fd, 1, read_pbuf) == PFE_OK,
		"Should be able to get this page.");

	/* Make sure the read page_buf is the same as what we wrote (write_pbuf).
	I.e. every byte matches. */
	mu_assert(strcmp(write_pbuf, read_pbuf) == 0,
		"The data read should match the data wrote.");

	free(write_pbuf);
	free(read_pbuf);

	/* Clean up. */
	pf_close_file(fd);
	pf_destroy_file(filename);

	return NULL;
}

/* pf_get_next_page() */

char *test_get_next_page_should_return_eof_if_pagenum_is_neg_1_and_the_file_is_empty()
{	
	pf_init();
	
	/* Create an empty file. */
	char *filename = TEST_TEMP_ROOT "file_with_no_pages.pf";
	int ret_code = pf_create_file(filename);
	debug("pf_open_file() return code: %d", ret_code);
	mu_assert(ret_code == PFE_OK, "Should be able to create file.");

	/* Open the file. */
	int fd = pf_open_file(filename);
	
	/* Access the first page of the file and test return code. */
	char *pagebuf = NULL;
	int pagenum = -1;
	ret_code = pf_get_next_page(fd, &pagenum, pagebuf);
	debug("pf_get_next_page() return code: %d", ret_code);
	debug("PFE_EOF: %d", PFE_EOF);
	mu_assert(ret_code == PFE_EOF, "Should return end of file if we try to access the first page of an empty file.");

	/* Clean up. */
	unlink(filename);

	/* Make sure the file is gone. */
	int perms = 0666;
	mu_assert(open(filename, O_RDONLY, perms) == -1, "Should not be able to open a file that does not exists.");

	/* Clean up. */
	pf_close_file(fd);
	pf_destroy_file(filename);
	
	return NULL;
}

char *test_get_next_page_should_return_first_page_if_pagenum_is_neg_1_and_the_file_is_not_empty()
{
	pf_init();
	
	/* Create a new file. */
	char *filename = TEST_TEMP_ROOT "file_with_no_pages.pf";
	int ret_code = pf_create_file(filename);
	debug("pf_open_file() return code: %d", ret_code);
	mu_assert(ret_code == PFE_OK, "Should be able to create file.");

	/* Open the file. */
	// int fd = pf_open_file(filename);

	/* Add 3 entries to the file. */


	/* CONTINUE AFTER I HAVE WRITTEN THE CODE TO ALLOCATE PAGES TO A FILE. */

	/* Access the first page of the file and test return code. */

	/* Clean up. */


	/*
	how do i want to test this?

	create a file with 3 entries, all with data.
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
	
	/* Clean up. */
	pf_close_file(pf_get_fd_of_filename(filename));
	pf_destroy_file(filename);

	return NULL;
}

char *all_tests()
{
	mu_suite_start();
	
	/* pf_create_file() */
	mu_run_test(test_pf_create_file_should_fail_if_file_exists);
	mu_run_test(test_pf_create_file_should_work_with_valid_input);

	/* pf_open_file() */
	mu_run_test(test_pf_open_file_should_fail_if_file_does_not_exists);
	mu_run_test(test_pf_open_file_should_fail_if_we_open_the_same_file_twice);

	/* pf_get_this_page() */


	/* pf_write_page() */
	// mu_run_test(test_write_page);

	/* pf_get_next_page() */
	// mu_run_test(test_get_next_page_should_return_eof_if_pagenum_is_neg_1_and_the_file_is_empty);
	// mu_run_test(test_get_next_page_should_return_first_page_if_pagenum_is_neg_1_and_the_file_is_not_empty);


	return NULL;
}

RUN_TESTS(all_tests);