Makefile Instructions:
To clean the folder, use the command "make clean" in the root directory (/proj3). This command will clean proj3 along with the /test subdirectory.

IMPORTANT: The test subdirectory's makefile requires that the user change the "fPATH" field to the file path of the proj3 folder in the computer being used. 
Also note that a copy of memory.h was placed in the test subdirectory. Moving this file to a different directory will cause the program compilation to fail. 
Also note that the current makefile is made to work with a test.c file, and this readme will go over the tests in this file (as provided by Raju) and their results. Additional test files created by this group have also been provided, and can be run by renaming them to test.c or by editing the makefile.

Use "make" in the root directory /proj3 to create the "memory" executable. Note that some additional information may be displayed on the terminal concerning the activity of the makefile, since the root directory makefile calls a separate makefile in the proj3/test subdirectory.

To run a test, enter ./test # in the terminal in the proj3/test directory. # is a number from 1 to 6, one for each of the test functions contained in the test.c file. 

Professor Raju's test.c file in our proj3/test folder has been slightly modified from its original form. It required us to initialize and finalize the allocator in main, as test.c did not originally do this. Additionally, we also had to include the memory.h header file.

The following is an explanation of each of the tests and our results:

./test 1: Makes an allocation with a size of 32 bytes for a pointer of char* type.Then assigns some value through the pointer, outputs this value, and then deallocates it once. Running ./test 1 prints out the characters that were stored properly.

./test 2: Makes an allocation with a size of 32 bytes and deallocates it twice. As expected, running ./test 2 produces a double free error message.

./test 3: Makes allocations with sizes of 32 bytes,64 bytes,128 bytes,192 bytes, etc. as well as large requests (e.g., 16 pages) and deallocates them. No errors are produced in the test.c file provided to us. Our own test3.c file prints success and error messages for allocate and deallocate based on their return values, and running this file shows that the program did successfully allocate and deallocate those bytes.

./test 4: Loops 20 times and in each loop makes an allocation with a size of 32 bytes. When the loop is over, they are deallocated. No errors are produced when the test.c file provided to us is run. Our test4.c file prints the memory allocation addresses along with a message whenever a successful allocation is made. Running this file shows that the program did successfully make all 20 allocations and 20 deallocations.

./test 5: Creates 10 threads and in each thread loops 100,000 times and in each loop makes an allocation with a size of 256 bytes.Then, all of them are deallocated. Running ./test 5 first produces a list of the created threads as evidence that this portion of the test worked. However the test5 function in test.c did not appear to work properly, even when substituting allocate and deallocate for malloc and free. When we ran this, we encountered many errors when the deallocation portion of this test began. We modified this test in an attempt to fix these errors. We have also provided our own test5.c file that we had previous written, so we will instead talk about the results from this file instead. Our test5.c file contains locks to control when a thread may allocate and deallocate memory. Each time an allocation is made, we also check that the returned memory address is different from all other addresses allocated by the threads. Using this method, we were able to safely make all the required allocations and deallocations.

./test 6: Continuously makes allocations of a random size from 1 byte to 256 bytes and counts the total number of bytes allocated until an allocation fails. The sum of all these bytes is outputted as the result. Running ./test 6 returned a value of 136MB in a little over 4.5 minutes (271.01 seconds).
