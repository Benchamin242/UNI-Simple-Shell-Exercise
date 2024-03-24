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
    
    loadHistory();

    while(1){
        //Gets the current working directory 
        char currentDir[150];
        getcwd(currentDir,150);

        //Prints the user prompt
        printf(setTerminalBlue"%s|-o-| "resetTerminalColour,currentDir);

        //Gets the user input
        char input[512];
        char* fgetsResult = fgets(input, 511, stdin);
        
        //Exits the shell when CTRL+D or 'exit' is entered
        if((fgetsResult==NULL)|(strcmp(input,"exit\n")==0)){
            printf("\n");
            char* argv[51] = {"setpath",savedPath};
            setPath(argv);
            saveHistory();
            saveAlias();
            printf(setTerminalBlue"Goodbye!\n"resetTerminalColour);

            exit(EXIT_SUCCESS);
        }

        //Makes sure the input isn't just a '\n'
        if (strcmp(input,"\n")!=0)
        {
            addToHistory(input);
            Tokeniser(input);
        }
    }
}
