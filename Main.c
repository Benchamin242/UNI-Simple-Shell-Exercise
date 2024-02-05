#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>  
#include <sys/types.h>  
#include <sys/wait.h>  
#include <errno.h>


char* UserPrompt();
void forkAndExec(char commands[50][511]);
int internalCommand(char* argv[51]);
void getPath();
void setPath(char* pathString);
void changeDirectory(char* argv[51]);
char* delimiters=" \t<>|;&\n";

int main(){
    while(1){
        char* input=UserPrompt();
        if (!(strcmp(input,"\n")==0))
        {
            //tokenizes string and stores it
            char commands[50][511] = {""};
            char* token=strtok(input, delimiters);
            int i=0;

            while(token!=NULL){
                if((strcmp(token,"\n")!=0)){
                    printf("Command entered: '%s'\n",token);                  
                }
                else{
                    //To keep the output consistent 
                    printf("\n");
                }

                strcpy(commands[i], token);
                i++;
                token=strtok(NULL, delimiters);  
            }

            forkAndExec(commands);
        }
    }
}

//Returns the users' input, or quits if 'exit' is inputted or CTRL+D is detected
char* UserPrompt(){
    char* input=(char *)malloc(512 * sizeof(char));
    printf("\n|-o-| ");
    char* fgetsResult = fgets(input, 511, stdin);

    if((fgetsResult==NULL)|(strcmp(input,"exit\n")==0)){
        printf("\n");
        exit(0);
    }

    return input;
}

//Helper function to fork and exec
void forkAndExec(char commands[50][511]){
    __pid_t p=fork();
    if(p<0){
        printf("Fork Fail");
    }
    else if(p==0)
    {
        /*execvp(commands[0], commands);
        perror("execvp");  // Print an error message if execvp fails
        exit(EXIT_FAILURE);  // Terminate child process upon execvp failure */

        //loops through commands in to an array of pointers argv
        char *argv[51] = {""}; 

        for (int i = 0; i < 50; ++i) {
            argv[i] = commands[i];
            if (commands[i][0] == '\0') {
                break;  
            }
        }

        int argCount = -1;

        //argv input checker 
        for (int i = 0; argv[i] != NULL && argv[i][0] != '\0'; ++i) {
            printf("Argument %d: %s\n", i, argv[i]);

            if(strcmp(argv[i],"")!=0){
                argCount++;
            }
        }

        argv[strlen(argv[0]) - 1] = '\0';
        argv[argCount+1] = NULL;

        if(!internalCommand(argv)){
            execvp(argv[0], argv);
        }

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