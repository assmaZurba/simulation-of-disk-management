//written by Assma zurba

#include <string>
#include <map>
#include <fcntl.h>
#include <queue>
#include <assert.h>
#include <math.h>
#include <iostream>
#include <vector>
#include <string.h>

using namespace std;

#define DISK_SIZE 256
#define DISK_SIM_FILE "DISK_SIM_FILE.txt"

class FsFile {
    int file_size;
    int block_in_use; //num of bocks that the file uses.
    int index_block; //the num of the block that contains the blocks (in use) num.
    int block_size;

public:
    FsFile(int _block_size) {
        file_size = 0;
        block_in_use = 0;
        block_size = _block_size;
        index_block = -1;
    }
    int getfile_size(){
        return file_size;
    }int getblock_in_use(){
        return block_in_use;
    }int getblock_size(){
        return block_size;
    }int getindex_block(){
        return index_block;
    }void setfile_size(int _file_size){
        file_size=_file_size;
    }void setblock_in_use(int _block_in_use){
        block_in_use=_block_in_use;
    }
    void setblock_size(int _block_size){
        block_size =_block_size;
    }void setindex_block(int _index_block){
        index_block=_index_block;
    }
};

class FileDescriptor {
    string file_name;
    FsFile* fs_file;
    bool inUse; //true if the file is open, false otherwise.

public:
    FileDescriptor(string FileName, FsFile* fsi) {
        file_name = FileName;
        fs_file = fsi;
        inUse = true;
    }
    string getFileName() {
        return file_name;
    }
    FsFile* getFsFile() {
        return fs_file;
    }
    bool getInUse() {
        return inUse;
    }
    void setFileName(string file_name) {
        this->file_name=file_name;
    }
    void setFsFile(FsFile* fs_file) {
        this->fs_file=fs_file;
    }
    void setInUse(bool inUse) {
        this->inUse=inUse;
    }
};

class fsDisk {
    FILE *sim_disk_fd; //stream of the disk file.
    bool is_formated;
    int blockSize;
    int BitVectorSize;
    int numOfBlocks;
    int maxFileSize;
    int *BitVector; //indicate which block in the disk is free or not.
    map<string,FileDescriptor*> mainDir; // map that contains all the files that is written in the disk.
    map<int,FileDescriptor*> OpenFileDescriptors; //a map that contains all the files that is open.
    queue<int> fdQ; //a queue of the fd num that we can use.
    int fdMax; //the max fd that will be saved in the queue.

public:
    /*
     * the constructor of the class.
     * open the file disk and fill the fields.
     */
    fsDisk(){
        sim_disk_fd = fopen(DISK_SIM_FILE , "r+");
        assert(sim_disk_fd);
        for (int i=0; i < DISK_SIZE ; i++) {
            int ret_val = fseek ( sim_disk_fd , i , SEEK_SET );
            ret_val = fwrite( "\0" ,  1 , 1, sim_disk_fd);
            assert(ret_val == 1);
        }
        fflush(sim_disk_fd);
        BitVectorSize=0;
        is_formated = false;
        fdMax=0;
        fdQ.push(fdMax);
    }

    /*
     * the destructor of the class.
     * close the file disk and free all the allocated memory.
     */
    ~fsDisk();

    /*
     * find the min fd from the fd queue
     */
    int findMinFdFromQ();

    /*
     * print all the name of the crated files and theirs status.
     * print all the disk file.
     */
    void listAll();

    /*
     * delete every thing in the disk and set the block size in the disk.
     * initialize the BitVector array and count the num of blocks in the disk.
     * the default size of the block is 4.
     */
    void fsFormat( int blockSize =4 );

    /*
     * creat a new file, and open it (in use=1)
     * update the mainDir,OpenFileDes
     * return the fd in success, -1 in failed
     */
    int CreateFile(string fileName);

    /*
    * open file that is already exist
    * update the OpenFileDes
    * return the fd in success, -1 in failed
    */
    int OpenFile(string fileName);

    /*
     * close existed and opened file
     * update the OpenFileDes
     * return file name in success, -1 in failed
     */
    string CloseFile(int fd);

    /*
     * write to file:
     * if it is the first write: find an empty block of the block's index, and blocks to write the file.
     * else: find the index block of the file than find a blocks to write in and update the block index.
     * return 1 in success, return -1 in failed
     */
    int WriteToFile(int fd, char *buf, int len );

    /*
     * delete the file:
     * delete the file from the disk
     * update the mainDir, OpenFileDes, BitVictor
     * return 1 in success, return -1 in failed
     */
    int DelFile( string FileName );

    /*
     * read from the file:
     * fd: the file desc, buf: the read section will be written here, len: the length of the section that will be read
     * return 1 in success, return -1 in failed
     */
    int ReadFromFile(int fd, char *buf, int len );

};
int main() {
    int blockSize;
    int direct_entries;
    string fileName;
    char str_to_write[DISK_SIZE];
    char str_to_read[DISK_SIZE];
    int size_to_read;
    int _fd;

    fsDisk *fs = new fsDisk();
    int cmd_;
    while(1) {
        cin >> cmd_;
        switch (cmd_)
        {
            case 0:   // exit
                delete fs;
                exit(0);
                break;

            case 1:  // list-file
                fs->listAll();
                break;

            case 2:    // format
                cin >> blockSize;
                fs->fsFormat(blockSize);
                break;

            case 3:    // creat-file
                cin >> fileName;
                _fd = fs->CreateFile(fileName);
                cout << "CreateFile: " << fileName << " with File Descriptor #: " << _fd << endl;
                break;

            case 4:  // open-file
                cin >> fileName;
                _fd = fs->OpenFile(fileName);
                cout << "OpenFile: " << fileName << " with File Descriptor #: " << _fd << endl;
                break;

            case 5:  // close-file
                cin >> _fd;
                fileName = fs->CloseFile(_fd);
                cout << "CloseFile: " << fileName << " with File Descriptor #: " << _fd << endl;
                break;

            case 6:   // write-file
                cin >> _fd;
                cin >> str_to_write;
                fs->WriteToFile( _fd , str_to_write , strlen(str_to_write) );
                break;

            case 7:    // read-file
                cin >> _fd;
                cin >> size_to_read ;
                fs->ReadFromFile( _fd , str_to_read , size_to_read );
                cout << "ReadFromFile: " << str_to_read << endl;
                break;

            case 8:   // delete file
                cin >> fileName;
                _fd = fs->DelFile(fileName);
                cout << "DeletedFile: " << fileName << " with File Descriptor #: " << _fd << endl;
                break;

            default:
                break;
        }
    }
}

/*
 * the destructor of the class.
 * close the file disk and free all the allocated memory.
 */
fsDisk::~fsDisk() {

    auto it1=OpenFileDescriptors.begin();
    while(it1!=OpenFileDescriptors.end()){
        auto itTemp=it1;
        ++it1;
        CloseFile(itTemp->first);
    }
    auto it2=mainDir.begin();
    while(it2!=mainDir.end()){
        auto itTemp=it2;
        ++it2;
        DelFile(itTemp->first);
    }
    if(is_formated == true)
        delete [] BitVector;
    fclose(sim_disk_fd);
}
// ------------------------------------------------------------------------

/*
 * find the min fd from the fd queue
 */
int fsDisk::findMinFdFromQ(){
    int min_value = fdQ.front();
    fdQ.pop();
    size_t size = fdQ.size();
    if(size==0){
        fdMax++;
        fdQ.push(fdMax);
    }
    while (size-- > 0) {
        int x = fdQ.front();
        fdQ.pop();
        if (x < min_value){
            fdQ.push(min_value);
            min_value = x;
        } else{
            fdQ.push(x);
        }
    }
    return min_value;
}
// ------------------------------------------------------------------------

/*
 * print all the name of the crated files and theirs status.
 * print all the disk file.
 */
void fsDisk::listAll() {
    int i = 0;
    for (auto const &pair: mainDir) {
        cout << "index: " << i << ": FileName: " << pair.first<<  " , isInUse: "<< pair.second->getInUse() << endl;
        i++;
    }
    char bufy;
    cout << "Disk content: '";
    for (i = 0; i < DISK_SIZE; i++)
    {
        cout << "(";
        int ret_val = fseek(sim_disk_fd, i, SEEK_SET);
        ret_val = fread(&bufy, 1, 1, sim_disk_fd);
        cout << bufy;
        cout << ")";
    }
    cout << "'" << endl;
}
// ------------------------------------------------------------------------

/*
 * delete every thing in the disk and set the block size in the disk.
 * initialize the BitVector array and count the num of blocks in the disk.
 * the default size of the block is 4.
 */
void fsDisk::fsFormat( int blockSize ) {
    if(blockSize<1 || blockSize>DISK_SIZE){
        cout<<"invalid number!"<<endl;
        return;
    }
    if(is_formated== true){
        auto it1=OpenFileDescriptors.begin();
        while(it1!=OpenFileDescriptors.end()){
            auto itTemp=it1;
            ++it1;
            CloseFile(itTemp->first);
        }
        auto it2=mainDir.begin();
        while(it2!=mainDir.end()){
            auto itTemp=it2;
            ++it2;
            DelFile(itTemp->first);
        }
        delete [] BitVector;
        while (!fdQ.empty())
            fdQ.pop();
        fdMax=0;
        fdQ.push(fdMax);
    }
    this->blockSize=blockSize;
    maxFileSize=blockSize*blockSize;
    numOfBlocks=DISK_SIZE/blockSize;
    BitVector=new int [numOfBlocks];
    for (int i=0; i < numOfBlocks ; i++) {
        BitVector[i]=0;
    }
    BitVectorSize=0;
    is_formated=true;
    cout<<"FORMAT DISK: number of blocks: "<<numOfBlocks<<endl;
}
// ------------------------------------------------------------------------

/*
 * creat a new file, and open it (in use=1)
 * update the mainDir,OpenFileDes
 * return the fd in success, -1 in failed
 */
int fsDisk::CreateFile(string fileName) {
    //return -1 if the disk is not formatted yet or the file is already exist
    if(is_formated== false || mainDir.find(fileName)!= mainDir.end() )
        return -1;
    FsFile *fs_file=new FsFile(blockSize);
    FileDescriptor* fileDes=new FileDescriptor(fileName,fs_file);
    fileDes->setInUse(true);
    int fd =findMinFdFromQ();
    OpenFileDescriptors.insert({fd,fileDes});
    mainDir.insert({fileName,fileDes});
    return fd;
}
// ------------------------------------------------------------------------

/*
* open file that is already exist
* update the OpenFileDes
* return the fd in success, -1 in failed
*/
int fsDisk::OpenFile(string fileName) {
    //return -1 if the disk is not formatted yet, the file is not exist or the file is already open
    if(is_formated== false ||mainDir.find(fileName)== mainDir.end() || mainDir [fileName]->getInUse()== true){
        return -1;
    }
    int fd=findMinFdFromQ();
    mainDir[fileName]->setInUse(true);
    OpenFileDescriptors.insert({fd,mainDir[fileName]});
    return fd;
}
// ------------------------------------------------------------------------

/*
 * close existed and opened file
 * update the OpenFileDes
 * return file name in success, -1 in failed
 */
string fsDisk:: CloseFile(int fd) {
    //return error -1 if the disk is not formatted yet or the file is not open
    if( is_formated== false || OpenFileDescriptors.find(fd)== OpenFileDescriptors.end() ){
        return "-1";
    }
    string fileName=OpenFileDescriptors[fd]->getFileName();
    OpenFileDescriptors[fd]->setInUse(false);
    OpenFileDescriptors.erase(fd);
    fdQ.push(fd);
    return fileName;
}
// ------------------------------------------------------------------------

/*
 * write to file:
 * if it is the first write: find an empty block of the block's index, and blocks to write the file.
 * else: find the index block of the file than find a blocks to write in and update the block index.
 * return 1 in success, return -1 in failed
 */
int fsDisk::WriteToFile(int fd, char *buf, int len ) {
    //return error -1 if the disk is not formatted yet, the file is not open or the disk is full
    if(is_formated== false || OpenFileDescriptors.find(fd)== OpenFileDescriptors.end() || OpenFileDescriptors[fd]->getFsFile()->getfile_size()==maxFileSize|| BitVectorSize==numOfBlocks||len==0)
        return -1;
    queue<int> fileBlocks;
    FileDescriptor* fileDes=OpenFileDescriptors [fd];
    int numOfNeededBlocks=0;
    if(len>maxFileSize-OpenFileDescriptors[fd]->getFsFile()->getfile_size())
        len=maxFileSize-OpenFileDescriptors[fd]->getFsFile()->getfile_size();
    int realLength=len;
    if(fileDes->getFsFile()->getfile_size()==0){//first write
        if(BitVectorSize+1==numOfBlocks)//there is a place just for the index block!
            return -1;
        for (int i=0; i < numOfBlocks ; i++) {
            //finding the index block
            if(BitVector[i]==0){
                fileDes->getFsFile()->setindex_block(i);
                BitVector[i]=1;
                BitVectorSize++;
                break;
            }
        }
    }
    else if((fileDes->getFsFile()->getfile_size()%blockSize)!=0){//there is a space in the old last block
        unsigned char c[2];
        fseek(sim_disk_fd, (fileDes->getFsFile()->getindex_block()*blockSize)+fileDes->getFsFile()->getblock_in_use()-1, SEEK_SET);
        fread(c, sizeof (char ), 1, sim_disk_fd);
        fseek(sim_disk_fd, ((int)c[0]*blockSize)+(fileDes->getFsFile()->getfile_size()%blockSize), SEEK_SET);
        if(len>blockSize-(fileDes->getFsFile()->getfile_size()%blockSize)){
            fwrite(buf,sizeof (char ),blockSize-(fileDes->getFsFile()->getfile_size()%blockSize),sim_disk_fd);
            buf+=blockSize-(fileDes->getFsFile()->getfile_size()%blockSize);
            len-=blockSize-(fileDes->getFsFile()->getfile_size()%blockSize);
        }
        else{
            fwrite(buf,sizeof (char ),len,sim_disk_fd);
            buf+=len;
            len=0;
        }
    }
    if(len%blockSize==0)
        numOfNeededBlocks=(len/blockSize);
    else
        numOfNeededBlocks=(len/blockSize)+1;
    if(numOfNeededBlocks>numOfBlocks-BitVectorSize)
        numOfNeededBlocks=numOfBlocks-BitVectorSize;
    int k=numOfNeededBlocks;
    for (int i=0; i < numOfBlocks && k>0 ; i++) {
        //find the file blocks
        if(BitVector[i]==0 ){
            fileBlocks.push(i);
            BitVector[i]=1;
            BitVectorSize++;
            k--;
        }
    }
    for(int i=0;i<numOfNeededBlocks;i++){
        //write in the blocks
        int blockInd=fileBlocks.front();
        fileBlocks.pop();
        unsigned char bInd[2];
        bInd[0]=(unsigned char)blockInd;
        fseek(sim_disk_fd, (fileDes->getFsFile()->getindex_block()*blockSize)+fileDes->getFsFile()->getblock_in_use()+i, SEEK_SET);
        fwrite(bInd,sizeof (char ),1,sim_disk_fd);
        fseek(sim_disk_fd, (blockInd*blockSize), SEEK_SET);
        if(len>blockSize){
            fwrite(buf,sizeof (char ),blockSize,sim_disk_fd);
            buf+=blockSize;
            len-=blockSize;
        }
        else{
            fwrite(buf,sizeof (char ),len,sim_disk_fd);
            buf+=len;
            len=0;
            break;
        }
    }
    fileDes->getFsFile()->setblock_in_use(fileDes->getFsFile()->getblock_in_use()+numOfNeededBlocks);
    fileDes->getFsFile()->setfile_size(fileDes->getFsFile()->getfile_size()+realLength-len);
    return realLength-len;
}
// ------------------------------------------------------------------------

/*
 * delete the file:
 * delete the file from the disk
 * update the mainDir, OpenFileDes, BitVictor
 * return 1 in success, return -1 in failed
 */
int fsDisk::DelFile( string FileName ) {
    //return error -1 if the disk is not formatted yet, the file is not exist
    if(is_formated== false ||mainDir.find(FileName)==mainDir.end() || mainDir[FileName]->getInUse()==true)
        return -1;
    queue<int> fileBlocks;
    if(mainDir[FileName]->getFsFile()->getindex_block()!=-1){
        BitVector[mainDir[FileName]->getFsFile()->getindex_block()]=0;
        BitVectorSize--;
    }
    for(int i=0;i<mainDir[FileName]->getFsFile()->getblock_in_use();i++){
        unsigned char c[1];
        fseek(sim_disk_fd, (mainDir[FileName]->getFsFile()->getindex_block()*blockSize)+i, SEEK_SET);
        fread(c, sizeof (char ), 1, sim_disk_fd);
        fileBlocks.push(( int ) c[0]);
        BitVector[ ( int ) c[0] ]=0;
        BitVectorSize--;
        fseek(sim_disk_fd, (mainDir[FileName]->getFsFile()->getindex_block()*blockSize)+i, SEEK_SET);
        fwrite( "\0" ,  sizeof (char)  , 1, sim_disk_fd);
    }
    for(int i=0;i<mainDir[FileName]->getFsFile()->getblock_in_use();i++){
        int blockInd=fileBlocks.front();
        fileBlocks.pop();
        for(int j=0;j<blockSize;j++){
            fseek(sim_disk_fd, (blockInd*blockSize)+j, SEEK_SET);
            fwrite( "\0" ,  sizeof (char) , 1, sim_disk_fd);
        }
    }
    delete mainDir[FileName]->getFsFile();
    delete mainDir[FileName];
    mainDir.erase(FileName);
    return 1;
}
// ------------------------------------------------------------------------

/*
 * read from the file:
 * fd: the file desc, buf: the read section will be written here, len: the length of the section that will be read
 * return 1 in success, return -1 in failed
 */
int fsDisk::ReadFromFile(int fd, char *buf, int len ) {
    //return error -1 if the disk is not formatted yet, the file is not open
    buf[0]='\0';
    if(is_formated== false ||OpenFileDescriptors.find(fd)==OpenFileDescriptors.end())
        return -1;
    queue<int> fileBlocks;
    if(len>OpenFileDescriptors[fd]->getFsFile()->getfile_size())
        len=OpenFileDescriptors[fd]->getFsFile()->getfile_size();
    int realLen=len;
    int numOfNeededBlocks=0;
    if(len%blockSize==0)
        numOfNeededBlocks=(len/blockSize);
    else
        numOfNeededBlocks=(len/blockSize)+1;
    for(int i=0;i<numOfNeededBlocks;i++){
        unsigned char c[1];
        fseek(sim_disk_fd, (OpenFileDescriptors[fd]->getFsFile()->getindex_block()*blockSize)+i, SEEK_SET);
        fread(c, sizeof (char ), 1, sim_disk_fd);
        fileBlocks.push(( int ) c[0] );
    }
    for(int i=0;i<numOfNeededBlocks;i++){
        int blockInd=fileBlocks.front();
        fileBlocks.pop();
        char buffer[blockSize];
        buffer[0]='\0';
        fseek(sim_disk_fd, (blockInd*blockSize), SEEK_SET);
        if(len>blockSize){
            fread(buffer, sizeof (char ), blockSize, sim_disk_fd);
            strncat(buf,buffer,blockSize);
            len-=blockSize;
        }
        else{
            fread(buffer, sizeof (char ), len, sim_disk_fd);
            strncat(buf,buffer,len);
            len=0;
            break;
        }
    }
    return realLen;
}
// ------------------------------------------------------------------------
