//+
// File:	shell.c
//
// Purpose:	This program implements a simple shell program. It does not start
//		processes at this point in time. However, it will change directory
//		and list the contents of the current directory.
//
//		The commands are:
//		   cd name -> change to directory name, print an error if the directory doesn't exist.
//		              If there is no parameter, then change to the home directory.
//		   ls -> list the entries in the current directory.
//			      If no arguments, then ignores entries starting with .
//			      If -a then all entries
//		   pwd -> print the current directory.
//		   exit -> exit the shell (default exit value 0)
//
//		if the command is not recognized an error is printed.
//-

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <pwd.h>
#include <ctype.h>

#define CMD_BUFFSIZE 1024
#define MAXARGS 10

int idx=0;
int argsidx=0;

//declare all functions to be used in the program
int splitCommandLine(char * commandBuffer, char* args[], int maxargs);
void doCommand(char * args[], int nargs);
void exit();
void pwdFunc(char* args[], int nargs);
void cdFunc(char* args[], int nargs);
void lsFunc(char* args[], int nargs);
int isDot(const struct dirent * d);

int main() {

    char commandBuffer[CMD_BUFFSIZE];
    char *args[MAXARGS];

    // print prompt.. fflush is needed because
    // stdout is line buffered, and won't
    // write to terminal until newline
    printf("%%> ");
    fflush(stdout);

	//printf("Hello World");


    while(fgets(commandBuffer,CMD_BUFFSIZE,stdin) != NULL){

	// Remove newline at end of buffer
	// TODO Step 2: remove newline from end of buffer


	//the following loop traverses throught the entire string to replace any newline charectors with a terminating (NULL) character
	int j;
	int x=sizeof(commandBuffer)/sizeof(char);
	for (j=0;j<x;j++){
		if (commandBuffer[j]=='\n'){
			commandBuffer[j]='\0';
		}
	}
	

	// Split command line into words.
	// TODO Step 2: call splitCommandLine with the right parameters

	int nargs = splitCommandLine(commandBuffer, args, MAXARGS); //call to the splitCommandLine function. Takes in commandBuffer array, args pointer array, and an integer for the maximum size of the args array. Int nargs stores the return from the function and is the number of arguments in the commandBuffer

	// Debugging for step 2
	printf("%d\n", nargs);
	int i;
	for (i = 0; i < nargs; i++){
	printf("%d: %s\n",i,args[i]);
	}

	// Execute the command
	// Remember to check if there is a command (i.e. value of nargs)
	// TODO: Step 3 call doCommand with the right arguments

	doCommand(args, nargs);//call to the doCommand function. Takes in pointer to the array of arguments and an integer with the number of arguments

	// print prompt
	printf("%%> ");
	fflush(stdout);


    }
}

////////////////////////////// String Handling ///////////////////////////////////

//+
// Function:	skipChar
//
// Purpose:	This function skips over a given char in a string
//		For security, the function will not skip null chars.
//
// Parameters:
//    charPtr	Pointer to string
//    skip	character to skip
//
// Returns:	Pointer to first character after skipped chars.
//		Identity function if the string doesn't start with skip,
//		or skip is the null character
//-

char * skipChar(char * charPtr, char skip){
 //function takes in pointer to first instance of the skip charector in the string and the skip charector. The function then countinues to check each element of the array starting at the charPtr until it reaches a character that is not the skip charater (this would must then be the start of the next word or the end of the string). The function then returns a pointer to the first letter of the next word. 

	if (skip==' '){
		while((*charPtr == ' ')||(*(charPtr+1)==' ')){//increment the charPtr while that charater or the follwoing one is a space
			charPtr++;
		}
		return charPtr;
	}else if(skip=='\0'){
		return charPtr;//return charPtr if skip charector is '\0'
	}else{
		while((*charPtr!=' ')&&(*charPtr!='\0')){
			charPtr++;
		}
		return charPtr;
	}

}

//+
// Function:	splitCommandLine
//
// Purpose:	This splits a string into an array of strings.
//		The array is passed as an argument. The string
//		is modified by replacing some of the space characters
//		with null characters to terminate each of the strings.
//
// Parameters:
//	commandBuffer	The string to split
//	args		An array of char pointers
//	maxargs		Size of array args (max number of arguments)
//
// Returns:	Number of arguments (< maxargs).
//
//-

int splitCommandLine(char * commandBuffer, char* args[], int maxargs){
   // function splits commandBuffer into single words using strchr and the skipChar function
	
	int i=0;
	while(*commandBuffer!='\0'){//countinue while current index of commandBuffer is not terminating charector
		args[i++]=commandBuffer=skipChar(commandBuffer, ' ');//use skipChar to skip spaces and store pointer to first letter of each word/command in args array
		char* end=strchr(commandBuffer, ' ');//use strchr to get a pointer to the first occurance of a space
		if (!end){
			break;
		}
		*end='\0';
		commandBuffer=++end;
	}
	if(i>MAXARGS){//print an error if user inputs more arguments than can be stores in the args array
		printf("error, too many arguments");
	}else{
		return i;//return the number of arguments in args (nargs)
	}

}


////////////////////////////// Command Handling ///////////////////////////////////

// typedef for pointer to command handling functions

// TODO STEP 3a write the typedef for the function handler pointer to
// use in the structure immediately below.
// See the description of the function prototypes at the bottom of
// the file in the comments.


// cmdStruct type:
// Associates a command name with a command handling function

//typedef void(*Other)(char* args[], int nargs);

// TODO STEP 3b use the typedef above (Step 3a) to make a two element
// struct. The first is a char * for the name of the command
// the other is a function pointer for the command handling function
// typedef struct {
//
// } typename;

typedef struct {//create structure to associate a command name with a corresponding function pointer 
	char* cmdName;
	void(*cmdFunction)(char* [], int);
}cmdStruct;

// prototypes for command handling functions
// TODO STEP 4b,6: add a function prototype
// for each command handling function

void exitFunc(){//exit function that exits program
	exit(0);
}



// Array that provides the list commands and functions
// must be terminated by {NULL, NULL} 
// in a real shell, this would be a hashtable.

// TODO Step 4a: add a global array of 
// the type above that contains initializers
// of strings and command handling funciton names
cmdStruct commands[] = {//create array of type cmdStruct (declared earlier) that stores the command name and a pointer to its corresponding function
	{"exit", exitFunc},
	{"pwd", pwdFunc},
	{"cd", cdFunc},
	{"ls", lsFunc},
	{NULL, NULL}
};


//+
// Function:	doCommand
//
// Purpose:	This command calls a command handling funciton from
//		the commands array based on the first argument passed
//		in the args array.
//
// Parameters:
//	args	command and parameters, an array of pointers to strings
//	nargs	number of entries in the args array
//
// Returns	nothing (void)
//-

void doCommand(char * args[], int nargs){
   // function that checks args against possible commands. if command is found, the function calls the appropriate command functions. If the command is not recognized, this function prints an appropriate error
	int i;
	if (nargs>0){//checks to see if the user inputted any arguments and simply returns if they did not
		for(i=0;commands[i].cmdName!=NULL;i++){//checks first argument of args array against commands in command array until the command is either found or the end of the command array is hit. 
			if (strcmp(commands[i].cmdName, args[0])==0){
				(*commands[i].cmdFunction)(args, nargs);//if command is found, call appropriate function and return
				return;
			}
		}
		fprintf(stderr, "Error: Command not found!\n");//if command is not recognize then print error
		
	}
	return;
}

//////////////////////////////////////////////////
//            command Handling Functions        //
//////////////////////////////////////////////////
// all command handling functions have the same
// parameter types and return values.
//////////////////////////////////////////////////

//+
// Function:	all handling functions
//
// Purpose:	this command performs the funcdtion
//		associated with the commands.
//		The name of the comomand is in first argument.
//	 	Since it is called by doCommand, it is known
//		that arg is at least one element long
//
// Parameters:
//	args	command and parameters, an array of pointers to strings
//	nargs	number of entries in the args array
//
// Returns	nothing (void)
//-

// TODO step 4b put command handling function for exit here
// e.g. void exitFunc(char * args[], int nargs){
// }

void pwdFunc(char* args[], int nargs){
	//pwd function that returns nothing. Function gets current working directory and prints it for the user
	char* cwd=getcwd(NULL, 0);
	printf("%s\n", cwd);
	free(cwd);//must free cwd memory
}

void cdFunc(char* args[], int nargs){
	// cd function returns nothing and is used to change current working directory
	struct passwd *pw=getpwuid(getuid());//gets pointer to password file entry of a user
	if (pw->pw_dir==NULL){//print error if getpwuid returns NULL
		fprintf(stderr, "Error, home directory not found\n");
	}else{//if return is not NULL then check the number of arguments that the user gave
		if (nargs==1){//if cd was called with no arguments, then set the current directory to the home directory
			int status=chdir(pw->pw_dir);
			if(status!=0){//prints error if directory cannot be changed
				fprintf(stderr, "Error, current directory could not be changed\n");
			}
		}else if (nargs==2){// if cd was called with an argument, then the function attempts to change to the directory that the user gave as an argument
			int status=chdir(args[1]);
			if(status!=0){//once again, an error is printed if the directory cannot be changed
				fprintf(stderr, "Error, current directory could not be changed\n");
			}
		}
	}
}

void lsFunc(char* args[], int nargs){
	//ls function that returns nothing and prints all entries in the current directory
	struct dirent** namelist;
	int numEnts=0;
	
	if (nargs==1){//if ls is called with no arguments then the function checks all entries to see if they have a preceding '.' which implies this is a hidden entry
		numEnts=scandir(".", &namelist, isDot, NULL);
	}else if(nargs==2){//if ls is called with arguments, the argument is checked to see if it is the keyword '-a'
		if(strcmp(args[1], "-a")==0){//if argument matches keyword, then all entries in current directory are gotten
			numEnts=scandir(".", &namelist, NULL, NULL);	
		}else{
			fprintf(stderr,"Error: Invalid argument: %s\n", args[1]);//if argument is not keyword '-a' then an error is printed
		}
	}
	int i;
	for(i=0;i<numEnts;i++){//print entries according to what was determined with hidden entries above
		printf("%s\n", namelist[i]->d_name);
	}
}

int isDot(const struct dirent * d){
	//helper function used to check if a string contains a preceding '.'
	if (d->d_name[0]=='.'){
		return 0;
	}else{
		return 1;
	}
}

// TODO step 6 put rest of command handling functions here
