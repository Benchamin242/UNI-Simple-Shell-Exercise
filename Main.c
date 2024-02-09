#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>  
#include <sys/types.h>  
#include <sys/wait.h>  
#include <errno.h>

//Used to colour the output of the terminal
#define setTerminalBlue "\x1b[34m"
#define resetTerminalColour "\x1b[0m"

//Command history struct
typedef struct{
    int commandNumber;
    char commandLine[512];
} CommandHistory;

CommandHistory history[20];
int historyCount;

int currentCommandNo = 0;

void forkAndExec(char commands[50][511]);
int internalCommand(char* argv[51]);
void getPath();
void setPath(char* pathString);
void changeDirectory(char* argv[51]);
char* delimiters=" \t<>|;&\n";
void addToHistory(char *command);
void displayHistory();
void getCommandFromHistory(char *command[51]);

int main(){
    char* const savedPath = getenv("PATH");

    chdir(getenv("HOME"));

    while(1){
        //Gets the current working directory 
        char currentDir[50];
        getcwd(currentDir,50);

        //Prints the user promt
        printf(setTerminalBlue"%s|-o-| "resetTerminalColour,currentDir);

        //Gets the user input
        char* input=(char *)malloc(512 * sizeof(char));
        char* fgetsResult = fgets(input, 511, stdin);
        
        //Exits the shell when CTRL+D or 'exit' is entered
        if((fgetsResult==NULL)|(strcmp(input,"exit\n")==0)){
            free(input);
            printf("\n");
            setPath(savedPath);
            printf(setTerminalBlue"Goodbye!\n"resetTerminalColour);
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
            addToHistory(*commands);
           
            forkAndExec(commands);
        }
        
        free(input);
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
    }
    return 0;
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
    printf(setTerminalBlue"PATH set to: "resetTerminalColour);
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

void addToHistory(char *command){
    if(historyCount < 20){
        currentCommandNo++;
        history[historyCount].commandNumber = currentCommandNo;
        strcpy(history[historyCount].commandLine, command);
        historyCount++;
    }
    else{
        // loops through array moving each history member foward 
        for(int i = 1; i<20 ; i++){
            history[i - 1].commandNumber = history[i].commandNumber;
            strcpy(history[i - 1].commandLine, history[i].commandLine);
        }
        history[19].commandNumber = currentCommandNo;
        strcpy(history[19].commandLine, command);
    }
}

void displayHistory(){
    // goes through all the current commands and displays them
    for(int i = 0; i < currentCommandNo; i++ ){
        printf("%d: %s\n", (i+1), history[i].commandLine);
    }
}

void getCommandFromHistory(char *command[51]) {
    int num;
    // this doesnt fully work but i dont know why its meant to see which thype of command is sent then send it back to the start    of the internal command and loop through it there but it isnt sending it back
    if (strcmp(command[0], "!!") == 0) {
        if (historyCount > 0) {
            internalCommand(history[historyCount - 1].commandLine); 
        } else {
            printf("Error: No commands in history\n");
        }
    } else if (strcmp(command[1], "-") == 0) {
        if (strlen(command[2]) > 0) {
            num = atoi(command[2]);
        } else if (strlen(command[3]) > 0) {
            num = atoi(command[3]);
        } else {
            printf("Error: Invalid history index\n");
            return;
        }
        if (num <= 0 || num > historyCount) {
            printf("Error: Invalid history index\n");
            return;
        }
        printf("%d\n", num);
        internalCommand(history[historyCount - num].commandLine); 
    } else {
        if (strlen(command[1]) > 0) {
            num = atoi(command[1]);
        } else if (strlen(command[2]) > 0) {
            num = atoi(command[2]);
        } else {
            printf("Error: Invalid history index\n");
            return;
        }
        if (num <= 0 || num > historyCount) {
            printf("Error: Invalid history index\n");
            return;
        }
        printf("%d\n", num);
        internalCommand(history[num].commandLine); 
    }
}



