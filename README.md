Run:
1) With sample test cases
./linearsearch.sh
./binarysearch.sh
./deletion.sh
./join1.sh
./join2.sh

2) With custom test cases
./linearsearch <input_filename> <query_filename>.txt <output_filename>
./binarysearch <sorted_input_filename> <query_filename>.txt <output_filename>
./deletion <sorted_input_filename> <query_filename>
./join1 <unsorted_input_file1name> <unsorted_input_file2name> <output_filename>
./join2 <unsorted_input_file1name> <sorted_input_file2name> <output_filename>

# COL362/632 Assignment 3

- Code
	+ Keep all your C++ source files inside the home directory.
	+ You are supposed to use the functions mentioned in file_manager.h. Read documentation.txt for details of what each function does.
	+ Go through sample_run.cpp to get an idea of how to use the file manager.
	+ The whole source code has been shared with you for your debugging purposes and if you wish to compile in a different environment.

- Testcases
	+ Sample test files have been added to testcases directory. Read the README.md file for details on the testcases.

- Note
	+ Please don't try any funny business like by-passing the file_manager or storing the complete (large) file in your own memory. The file manager we provided must be used for every access to file.