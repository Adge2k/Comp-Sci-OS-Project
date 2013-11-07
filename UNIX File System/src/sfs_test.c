/****************************************************
  This is a simple interactive test program for use
  with the file system interface functions.   
  To use this program, compile it and link 
  it with your implementation of the
  file system functions, and with the blockio functions.

  The program will print out a list of available commands
  and then prompt for a command.  There is one command for
  each function in the file system interface.

  This program is very simple.  It does minimal error checking
   and recovery.  All data written to and read from files passes
   through a single buffer (io_buffer).  Only printable ASCII
   characters may be written to files.  Non-printable characters
   read from files will not be displayed properly.

  You are free to make a copy of this program and to modify your
   copy for the purposes of testing your file system implementation.
******************************************************/
#include <stdio.h>

/*****************************************************
   templates for the sfs interface functions
******************************************************/

int
sfs_open(char *pathname);

int
sfs_read(int fd,
	 int start,
	 int length,
	 char *mem_pointer);

int
sfs_write(int fd,
	  int start,
	  int length,
	  char *mem_pointer);

int
sfs_readdir(int fd,
	    char *mem_pointer);

int
sfs_close(int fd);

int
sfs_delete(char *pathname);

int
sfs_create(char *pathname,
	   int type);

int
sfs_getsize(char *pathname);

int
sfs_gettype(char *pathname);

int
sfs_initialize(int erase);

int
loadDirectory();

int 
getNextEmptyBlk();



/*****************************************************
   Program constants
******************************************************/

/* This is the maximum number of bytes that can be read from
   or written to a file with a single file system call using this
   program.  Since files are limited
   to 512 bytes length, this should be sufficient. */
/* the format definition is necessary because macro substitutions
   do not take place within quoted strings */
#define MAX_IO_LENGTH   1024       /* I CHANGED IT FROM 512--->1024 */
#define IO_BUF_FORMAT   "%512s"

/* This is the maximum length of input strings (e.g., file names) read from
   the standard input.  This should be large enough for most purposes. */
/* the format definition is necessary because macro substitutions
   do not take place within quoted strings */
#define MAX_INPUT_LENGTH  512
#define INPUT_BUF_FORMAT  "%1024s"

/*****************************************************
   Global data structures
******************************************************/

/* buffer to hold commands read from standard input */
char command_buffer[MAX_INPUT_LENGTH+1];

/* buffer to hold data going to/from the disk */
char io_buffer[MAX_IO_LENGTH+1];

/* the following is used to hold string input parameters, such as file names */
char data_buffer_1[MAX_INPUT_LENGTH];
/* the following are used to hold integer input parameters */
int p1,p2,p3;

short int super_blk_buffer[128];

struct FileNamePair{
  char* Filename;
  int inumber;
  struct FilenamePair* next;
  struct FilenamePair* child;
  
};
typedef struct FilenamePair dEntry;

/*****************************************************
   main test routine
******************************************************/

main()
{
  int i;
  int retval;  /* used to hold return values of file system calls */

  /* do forever:
     1) print a list of available commands
     2) read a command
     3) read arguments for the command
     4) perform the requested operation
     5) display the results of the operation
  */
  while(1) {
    /* print a list of available commands */
    printf("\n");
    printf("o: open a file\n");
    printf("r: read from a file\n");
    printf("w: write to a file\n");
    printf("R: read from a directory\n");
    printf("c: close a file\n");
    printf("m: create (make) a new file\n");
    printf("d: delete a file\n");
    printf("s: get the size of a file\n");
    printf("t: get the type of a file\n");
    printf("i: initialize the file system\n");
    printf("q: quit - exit this program\n");
    /* read in the next command */
    printf("\nCommand? ");
    if (gets(command_buffer) == NULL) break;
    /* determine which command was requested */
    switch(command_buffer[0]) {
    case 'o':
      /* Open a file */
      printf("Enter full path name of file to open: ");
      scanf(INPUT_BUF_FORMAT,data_buffer_1);
      retval = sfs_open(data_buffer_1);
      if (retval >= 0) {
	printf("Open succeeded.  File Descriptor number is %d\n",retval);
      }
      else {
	printf("Error.  Return value was %d\n",retval);
      }
      break;
    case 'r':
      /* Read from a file */
      printf("Enter file descriptor number: ");
      scanf("%d",&p1);
      printf("Enter read start location: ");
      scanf("%d",&p2);
      printf("Enter number of bytes to read: ");
      scanf("%d",&p3);
      retval = sfs_read(p1,p2,p3,io_buffer);
      if (retval > 0) {
	printf("Read succeeded.\n");
	printf("The following data was read (only printable ASCII will display)\n");
	for(i=0;i<p3;i++) {
	  putchar(io_buffer[i]);
	}
	printf("\n");
      }
      else {
	printf("Error.  Return value was %d\n",retval);
      }
      break;
    case 'w':
      /* Write to a file */

      printf("Enter file descriptor number: ");
      scanf("%d",&p1);
      printf("Enter write start location: ");
      scanf("%d",&p2);
      printf("Enter number of bytes to write: ");
      scanf("%d",&p3);
      printf("This program allows only non-white-space, printable ASCII characters to be written to a file.\n");
      printf("Enter %d characters to be written: ",p3);
      scanf(IO_BUF_FORMAT,io_buffer);
      retval = sfs_write(p1,p2,p3,io_buffer);
      if (retval > 0) {
	printf("Write succeeded.\n");
	printf("Wrote %s to the disk\n",io_buffer);
      }
      else {
	printf("Error.  Return value was %d\n",retval);
      }
      break;
    case 'R':
      /* Read from a directory */
      printf("Enter file descriptor number: ");
      scanf("%d",&p1);
      while(retval>0|retval==NULL){	//added for loop
	retval = sfs_readdir(p1,io_buffer);
	if (retval > 0) {
	  printf("sfs_readdir succeeded.\n");
	  printf("Directory entry is: %s\n",io_buffer);
	}
	else if (retval == 0) {
	  printf("sfs_readdir succeeded.\n");
	  printf("No more entries in this directory\n");
	}
	else {
	  printf("Error.  Return value was %d\n",retval);
	}
      }
      break;
    case 'c':
      /* Close a file */
      printf("Enter file descriptor number: ");
      scanf("%d",&p1);
      retval = sfs_close(p1);
      if (retval > 0) {
	printf("sfs_close succeeded.\n");
      }
      else {
	printf("Error.  Return value was %d\n",retval);
      }
      break;
    case 'm':
      /* Create a new file */
      printf("Enter full path name of new file: ");
      scanf(INPUT_BUF_FORMAT,data_buffer_1);
      printf("Enter 0 for regular file, 1 for directory: ");
      scanf("%d",&p1);
      retval = sfs_create(data_buffer_1,p1);
      if (retval > 0) {
	printf("sfs_create succeeded.\n");
      }
      else {
	printf("Error.  Return value was %d\n",retval);
      }
      break;
    case 'd':
      /* Delete a file */
      printf("Enter full path name of file to delete: ");
      scanf(INPUT_BUF_FORMAT,data_buffer_1);
      retval = sfs_delete(data_buffer_1);
      if (retval > 0) {
	printf("sfs_delete succeeded.\n");
      }
      else {
	printf("Error.  Return value was %d\n",retval);
      }
      break;
    case 's':
      /* Get the size of a file */
      printf("Enter full path name of file: ");
      scanf(INPUT_BUF_FORMAT,data_buffer_1);
      retval = sfs_getsize(data_buffer_1);
      if (retval >= 0) {
	printf("sfs_getsize succeeded.\n");
	printf("size = %d\n",retval);
      }
      else {
	printf("Error.  Return value was %d\n",retval);
      }
      break;
    case 't':
      /* Get the type of a file */
      printf("Enter full path name of file: ");
      scanf(INPUT_BUF_FORMAT,data_buffer_1);
      retval = sfs_gettype(data_buffer_1);
      if (retval >= 0) {
	printf("sfs_gettype succeeded.\n");
	if (retval == 0) {
	  printf("file type is REGULAR\n");
	}
	else if (retval == 1) {
	  printf("file type is DIRECTORY\n");
	}
	else {
	  printf("file has unknown type %d\n",retval);
	}
      }
      else {
	printf("Error.  Return value was %d\n",retval);
      }
      break;
    case 'i':
      /* Initialize the file system */
      printf("Enter 1 to erase disk while initializing, 0 otherwise: ");
      scanf("%d",&p1);
      retval = sfs_initialize(p1);
      if (retval > 0) {
	printf("sfs_initialize succeeded.\n");
      }
      else {
	printf("Error.  Return value was %d\n",retval);
      }
      break;
    case 'q':
      /* Quit this program */
      break;
    default:
      printf("Unknown command: %s\n",command_buffer);
      break;
    }
    if (command_buffer[0] == 'q') break;
    /* cleanup the newline that remains after reading command parameter(s) */
    gets(command_buffer);
  }
}
      
int sfs_open(char *pathname){
  int inumber = parsePathname(pathname);
  get_block(inode[0][3], io_buffer);
  
}

int sfs_read(int fd, int start, int length, char *mem_pointer){
  
}

int sfs_write(int fd, int start, int length, char *mem_pointer){
  
}

int sfs_readdir(int fd, char *mem_pointer){
  int blocknum = inode[fd][3];
  get_block(blocknum, mempointer);
  
}

int sfs_close(int fd){
  
}

int sfs_delete(char *pathname){
  int inumber = parsePathname(pathname);
  if(inumber<0){
    printf("File %s does not exist", pathanme);
  }else{
    
  }
}

int sfs_create(char *pathname, int type){
  int inumber = parsePathnameAvail(pathname);
  if(inumber==-1){
    printf("File %s already exists\n", pathanme);
  }else if(inumber == -2){
    printf("Pathname %s is invalid\n", pathanme); 
  }else{
    int blocknum = getNextEmptyBlk();
    inode[inumber][1] = type;
    inode[inumber][2] = 0;
    inode[inumber][3] = blocknum;
    
  }
}

int sfs_getsize(char *pathname){
  int inumber = parsePathname(pathname);
  //printf("File Size of %s id %d\n", pathname, inode[inumber][2]);
  return inode[inumber][2];
}

int sfs_gettype(char *pathname){
  int inumber = parsePathname(pathname);
  char* FileType;
  if (inode[inumber][1]){
    FileType = "File";
  }else{
    FileType = "Folder";
  }
  printf("File Type of %s id %s\n", pathname, FileType );
}

int sfs_initialize(int erase){
  char buff[256];
  short int inode_table[64][4];
  int j;
  int i;
  if(erase){
    
  }else{
    short int disk_bitmap[512];
    for(i=0; i<8; i++){		//sets the first 2 blocks as full for storing the bitmap
      disk_bitmap[0]=1;
    }
    for(i = 8; i<512; i++){
      disk_bitmap[i]=0;
    }
    for(i=0;i<256;i++){
      buff[i] = disk_bitmap[i];
    }
    put_block(0, buff);
    for(i=256;i<512;i++){
      buff[i-256] = disk_bitmap[i];
    }
    put_block(1, buff);
    for(i = 0; i<64; i++){
      inode_table[i][0]=i;
    }
    inode_table[0][1] = 0;	//file type is folder
    inode_table[0][2] = 0;	//size of folder is zero - more if contains files
    inode_table[0][3] = 11;	//block number where file is located
  }
  get_block(0, io_buffer);
  for(i=0;i<64;i++){
    int tmp=0;
    for(j=1;j<5;j++){
      if(j%4 == 1){
	if(io_buffer[i*4+j]==1)
	  temp+=8;
      }else if(j%4 == 2){
	  if(io_buffer[i*4+j-1]==1)
	  temp+=4;
      }else if(j%4 == 3){
	  if(io_buffer[i*4+j-1]==1)
	  temp+=2;
      }else if(j%4 == 0){
	  if(io_buffer[i*4+j-1]==1)
	  temp+=1;
      }
    }
      super_blk_buffer[i]=temp;
    }
    get_block(1, io_buffer);
  for(i=0;i<64;i++){
    int tmp=0;
    for(j=1;j<5;j++){
      if(j%4 == 1){
	if(io_buffer[i*4+j]==1)
	  temp+=8;
      }else if(j%4 == 2){
	  if(io_buffer[i*4+j-1]==1)
	  temp+=4;
      }else if(j%4 == 3){
	  if(io_buffer[i*4+j-1]==1)
	  temp+=2;
      }else if(j%4 == 0){
	  if(io_buffer[i*4+j-1]==1)
	  temp+=1;
      }
    }
      super_blk_buffer[i*2]=temp;
    }
    loadDirectory();
  }
void loadDirectory(){
    
}
int getNextEmptyBlk(){
    int i = 11;
    int freeblknum;
    while(superblockbuffer[i]==15){
      i++;
    }
    int freeblksum = spuer_blk_buffer[i];
    if(freeblksum >= 8){
      if(freeblksum >= 12){
	if(freeblksum >= 14){
	  freeblknum = 4;
	}else{
	  freeblknum = 3;
	}
      }else{
	freeblknum = 2;
      }
    }else{
      freeblknum = 1;
    }
    return(i*4+freeblknum);
	  
}