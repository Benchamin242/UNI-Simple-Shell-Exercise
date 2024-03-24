#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>  
#include <sys/types.h>  
#include <sys/wait.h>  
#include <errno.h>
#include <ctype.h>
#include "InternalCommands.h"
#include "Alias.h"
#include "History.h"

char* delimiters=" \t<>|;&\n";

//Helper function to fork and exec
void forkAndExec(char* argv[51]){
    
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
                perror(argv[0]);  
                _exit(EXIT_FAILURE); 
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

    if(tryAlias(argv)){
        return 1;
    }
    else if(strcmp(argv[0],"getpath")==0){
        if(argv[1]!=NULL){
            printf("getpath: No arguments expected\n");
            return 1;
        }
        getPath();
        return 1;
    }
    else if(strcmp(argv[0],"setpath")==0){
        setPath(argv);
        return 1;
    }
    else if(strcmp(argv[0],"cd")==0){
        changeDirectory(argv);
        return 1;
    }
    else if(strcmp(argv[0],"history")==0){
        displayHistory();
        return 1;
    }
    else if(argv[0][0] == '!'){
        getCommandFromHistory(argv);
        return 1;
    }
    else if (strcmp(argv[0],"alias")==0){
        if(argv[1] == NULL && argv[2] == NULL){
            printAliases();
            return 1;
        }

        addAlias(argv);
        return 1;
    }
    else if (strcmp(argv[0],"unalias")==0){
        removeAlias(argv[1]);
        return 1;
    }

    return 0;
}

//Runs the 'getpath' internal command
void getPath(){
    printf("%s\n",getenv("PATH"));
}

//Runs the 'setpath' internal command
void setPath(char* argv[51]){
    char* pathString = argv[1];
    if(pathString == NULL){
        printf("setpath: No such file or directory\n");
        return;
    }

    if(argv[2]!=NULL){
        printf("setpath: Only one argument expected\n");
        return;
    }

    setenv("PATH",pathString,1);
    printf("PATH set to: ");
    getPath();
}

//Change directory function
void changeDirectory(char* argv[51]){
    //if no other arguments provided cd changes to home directory
    if(argv[1]==NULL){
        if(chdir(getenv("HOME"))!=0) {
            //if cd was unsuccessful error is printed
            perror("cd");
        }
    }
    else if(argv[2]!=NULL){
        printf("cd: Only one argument expetced\n");
    }
    else{
        if(chdir(argv[1])!=0) {
            //if cd was unsuccessful error is printed
            perror("cd");
        }
    }
}

void Tokeniser(char *command) {
    //char* commands =(char *)malloc(512 * sizeof(char));
    char commands[50][511] = {""}; 

    char *token = strtok(command, delimiters);
    int i = 0;

    while (token != NULL && i < 50) {
        strcpy(commands[i], token);
        i++;
        token = strtok(NULL, delimiters);
    }

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

    forkAndExec(argv);
}

void trim(char *str) {
    int i;
    int begin = 0;
    int end = strlen(str) - 1;

    while (isspace(str[begin])) {
        begin++;
    }

    while ((end >= begin) && isspace(str[end])) {
        end--;
    }

    for (i = begin; i <= end; i++) {
        str[i - begin] = str[i];
    }

    str[i - begin] = '\0';
}