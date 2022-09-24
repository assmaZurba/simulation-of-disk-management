# simulation-of-disk-managment
written by Assma zurba

in general:
A disk managing simulation.

the program contains a 3 class that called FsFile, FileDescriptor, fsDisk

FsFile class:

    the fields of the class are:
        int file_size:the file size.
        int block_in_use:the num of bocks that the file uses.
        int index_block:the num of the block that contains the blocks (in use) num.
        int block_size: the block size.

    the constructor of the class: FsFile(int _block_size):
        fill the fields of the class:
            file_size = 0
            block_in_use = 0
            block_size = _block_size
            index_block = -1

    functions:
        set/get for all the fields.

FileDescriptor class:

    the fields of the class are:
        string file_name: the file name
        FsFile * fs_file: a details about the file
        bool inUse: true if the file is open, false if the file is close

    the constructor of the class: FileDescriptor(string FileName, FsFile* fsi):
        fill the fields of the class:
            file_name = FileName;
            fs_file = fsi;
            inUse = true;

    functions:
        set/get for all the fields.

fsDisk class:

    the fields of the class are:
        FILE * sim_disk_fd:the stream of the disk file.
        bool is_formated:true if the disk formatted, false otherwise.
        int blockSize: the block size in the disk.
        int BitVectorSize: the num of blocks that are in use.
        int numOfBlocks: the num of blocks in the disk
        int maxFileSize: the max value of file size.
        int * BitVector: indicate which block in the disk is free or not (bitVector[the i-th block] =1 if the block in use, 0 otherwise).
        map<string,FileDescriptor*> mainDir: a map that contains all the files that is written in the disk,
            the key is the file name, the value is a pointer of the file descriptor.
        map<int,FileDescriptor*> OpenFileDescriptors: a map that contains all the files that is open,
            the key is the fd num, the value is a pointer of the file descriptor.
        queue<int> fdQ: a queue of the fd num that we can use.
        int fdMax: the max fd that will be saved in the queue.

    the constructor of the class: fsDisk():
        open the file disk and fill the fields.

    the destructor of the class: ~fsDisk():
        close the file disk and free all the allocated memory.

    functions:
        int findMinFdFromQ():
            find the min fd from the fd queue.
        void listAll():
            print all the name of the crated files and theirs status.
            print all the disk file.
        void fsFormat( int blockSize =4 ):
            delete every thing in the disk and set the block size in the disk.
            initialize the BitVector array and count the num of blocks in the disk.
            the default size of the block is 4.
        int CreateFile(string fileName):
            creat a new file, and open it (in use=1).
            update the mainDir,OpenFileDes.
            return the fd in success, -1 in failed.

        int OpenFile(string fileName):
            open file that is already exist.
            update the OpenFileDes.
            return the fd in success, -1 in failed.

        string CloseFile(int fd):
            close existed and opened file.
            update the OpenFileDes.
            return file name in success, -1 in failed.

        int WriteToFile(int fd, char *buf, int len ):
            if it is the first write: find an empty block of the block's index, and blocks to write the file.
            else: find the index block of the file than find a blocks to write in and update the block index.
            return 1 in success, return -1 in failed.

        int DelFile( string FileName ):
            delete the file from the disk.
            update the mainDir, OpenFileDes, BitVictor.
            return 1 in success, return -1 in failed.

        int ReadFromFile(int fd, char *buf, int len ):
             read from the file:
             fd: the file desc, buf: the read section will be written here, len: the length of the section that will be read.
             return 1 in success, return -1 in failed.

to compile:
    g++ main.cpp -o main
to run:
    ./main

note: run the code in the terminal!



