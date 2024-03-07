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

        addAlias(argv[1], argv[2]);
        return 1;
    }
    else if (strcmp(argv[0],"unalias")==0){
        removeAlias(argv[1]);
        return 1;
    }

    return tryAlias(argv);
}

//Runs the 'getpath' internal command
void getPath(){
    printf("%s\n",getenv("PATH"));
}

//Runs the 'setpath' internal command
void setPath(char* pathString){
    if(pathString == NULL){
        printf("setpath: No such file or directory\n");
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

int getnum(int startPos, char str[51]){
    int num_chars = 2;      // Number of characters you want to retrieve

    char number[num_chars + 1]; // Add 1 for null terminator
    char *ptr = str + startPos;
    for (int i = 0; i < num_chars && *ptr != '\0'; i++) {
        number[i] = *ptr;
        ptr++; // Move the pointer to the next character
    }
    number[num_chars] = '\0'; // Null terminate the string

    int num = 0;
    for (int i = 0; i < num_chars; i++) {
        if(number[i] != '\0'){
            num = num * 10 + (number[i]-'0');
        }
    }

    return num;
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