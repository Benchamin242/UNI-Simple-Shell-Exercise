#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>  
#include <sys/types.h>  
#include <sys/wait.h>  
#include <errno.h>

void forkAndExec(char commands[50][511]);
int internalCommand(char* argv[51]);
void getPath();
void setPath(char* pathString);
void changeDirectory(char* argv[51]);
char* delimiters=" \t<>|;&\n";

int main(){
    char* const savedPath = getenv("PATH");

    chdir(getenv("HOME"));

    while(1){
        //Gets the current working directory 
        char currentDir[50];
        getcwd(currentDir,50);

        //Prints the user promt
        printf("\n%s |-o-| ",currentDir);

        //Gets the user input
        char* input="";
        char* fgetsResult = fgets(input, 511, stdin);
        
        //Exits the shell when CTRL+D or 'exit' is entered
        if((fgetsResult==NULL)|(strcmp(input,"exit\n")==0)){
            setPath(savedPath);
            printf("\nGoodbye!\n");
            exit(EXIT_SUCCESS);
        }

        //Makes sure the input isn't just a '\n'
        if (!(strcmp(input,"\n")==0))
        {
            //Tokenizes string and stores it
            char commands[50][511] = {""};
            char* token=strtok(input, delimiters);
            int i=0;

            while(token!=NULL){
                strcpy(commands[i], token);
                i++;
                token=strtok(NULL, delimiters);  
            }

            forkAndExec(commands);
        }
    }
}

//Helper function to fork and exec
void forkAndExec(char commands[50][511]){

    //loops through commands in to an array of pointers argv
    char *argv[51] = {""}; 

    for (int i = 0; i < 50; ++i) {
        argv[i] = commands[i];
        if (commands[i][0] == '\0') {
            break;  
        }
    }

    int argCount = 0;

    //argv input checker 
    for (int i = 0; argv[i] != NULL && argv[i][0] != '\0'; ++i) {
        //printf("Argument %d: '%s'\n", i, argv[i]);

        if(strcmp(argv[i],"")!=0){
            argCount++;
        }
    }

    //Terminates the command string and array 
    argv[0][strlen(argv[0])] = '\0';
    argv[argCount] = NULL;   
	
	//Tries to run the internal command. If internalCommand returns 0, fork and exec. 
    if(!internalCommand(argv)){
        __pid_t p=fork();
		if(p<0){
		    printf("Fork Fail");
		}
		else if(p==0)//If child process
		{
		    execvp(argv[0], argv);

		    if(errno!=0){
		        perror("execvp");  
		        exit(EXIT_FAILURE); 
		    }

		    exit(EXIT_SUCCESS); 
		}
		else{
		    //ensures child process will execute first
		    wait(NULL);
		}
	}
}

//If argv contains an internal command, this executes it and returns 1. If not, returns 0.
int internalCommand(char* argv[51]){
    if(strcmp(argv[0],"getpath")==0){
        getPath();
        return 1;
    }
    else if(strcmp(argv[0],"setpath")==0){
        setPath(argv[1]);
        return 1;
    }
    else if(strcmp(argv[0],"cd")==0){
        changeDirectory(argv);
        return 1;
    }
    return 0;
}

//Runs the 'getpath' internal command
void getPath(){
    printf("%s\n",getenv("PATH"));
}

//Runs the 'setpath' internal command
void setPath(char* pathString){
    setenv("PATH",pathString,1);
    printf("\nPATH set to:\n");
    getPath();
}

//Change directory function
void changeDirectory(char* argv[51]){
    //if no other arguments provided cd changes to home directory
    if(argv[1]==NULL){
        if(chdir(getenv("HOME"))!=0) {
            //if cd was unsuccessful error is printed
            perror("chdir");
        }
    }
    else{
        if(chdir(argv[1])!=0) {
            //if cd was unsuccessful error is printed
            perror("chdir");
        }
    }
}
