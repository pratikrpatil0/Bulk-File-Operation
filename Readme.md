=============================================|
# Assignment 3 (Pratik and Neel)             | 
BULK FILE SYSTEM OPERATIONS                  |
=============================================|


-----------------------------------------------------------------------------------------------------

This C program provides functionality for various directory operations such as creating, deleting, renaming, appending, and logging.

=> Functionalities:- 

    Create: Create either a file or a directory.
    Delete: Delete a file or a directory recursively.
    Rename: Rename a file or a directory.
    Append: Append text or even numbers to files or directories.
    Log: Log operations performed by the program.

-----------------------------------------------------------------------------------------------------


## Usage

### Compilation

Compile the program using a C compiler. For example:

	gcc my_bfm.c -o my_bfm

We can even compile the program using the MakeFile using make command. 

### Execution

Run the compiled program with appropriate arguments. The general syntax is:

	./my_bfm [option] [arguments]

### Options:

    -c: Create a file or a directory. Provide the flag -f for file creation and -d for directory creation.
    -d: Delete a file or a directory.
    -r: Rename a file or a directory.
    -a: Append text or even numbers to a file or a directory. Specify the text or number and the path.
    -l: Log operations. Provide a filename to log.

-----------------------------------------------------------------------------------------------------


## Example Usage

### Creation
	./my_bfm -c -f <file.txt>
	./my_bfm -c -d <directory>
	
### Append
	./my_bfm -a <file_name/folder_name> -e <starting_number>
	./my_bfm -a <file_name/folder_name> -s <text>
	./my_bfm -a <file_name/folder_name> -e <starting_number> -s <text>

### Deletion
	./my_bfm -d <file_name/folder_name>
	
### Rename
	./my_bfm -r <old_file_name/old_folder_name> <new_file_name/new_folder_name>

### Log
	./my_bfm -l <log_file_name>


-----------------------------------------------------------------------------------------------------


### Arguments:

1. For create, delete, rename, and append operations, provide the path to the file or directory.
2. For append operation, provide either text or a number to append.
3. For rename operation, provide the new name.

### Notes

For Log function, we create a log folder by the name of "log", inside which the log file is contained. This particualr log file includes the log of all the operations done henceforth in the directory.

-----------------------------------------------------------------------------------------------------------



