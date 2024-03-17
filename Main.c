#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>  
#include <sys/types.h>  
#include <sys/wait.h>  
#include <errno.h>
#include <ctype.h>
#include "Alias.h"
#include "History.h"
#include "InternalCommands.h"

//Used to colour the output of the terminal
#define setTerminalBlue "\x1b[34m"
#define resetTerminalColour "\x1b[0m"


int main(){
    char* const savedPath = getenv("PATH");

    chdir(getenv("HOME"));

    loadAlias();        
    
    //below is LOADING FUNCTION
    loadHistory();

    while(1){
        //Gets the current working directory 
        char currentDir[150];
        getcwd(currentDir,150);

        //Prints the user prompt
        printf(setTerminalBlue"%s|-o-| "resetTerminalColour,currentDir);

        //Gets the user input
        char* input=(char *)malloc(512 * sizeof(char));
        char* fgetsResult = fgets(input, 511, stdin);
        
        //Exits the shell when CTRL+D or 'exit' is entered
        if((fgetsResult==NULL)|(strcmp(input,"exit\n")==0)){
            free(input);
            printf("\n");
            char* argv[51] = {"setpath",savedPath};
            setPath(argv);
            printf(setTerminalBlue"Goodbye!\n"resetTerminalColour);
            
            saveHistory();
            saveAlias();

            exit(EXIT_SUCCESS);
        }

        //Makes sure the input isn't just a '\n'
        if (!(strcmp(input,"\n")==0))
        {
            //Tokenizes string and stores it
            //char commands[50][511] = {""};

            addToHistory(input);

            //if input contains ("&&"), split input into two commands and run them separately
            if(strstr(input, "&&") != NULL){
                char *token = strtok(input, "&&");
                while(token != NULL){
                   
                    trim(token);
                    printf("Running: %s\n", token);
                    //Tokeniser(token);
                    token = strtok(NULL, "&&");
                }
            }
            else{

                Tokeniser(input);
            }
        }
    }
}