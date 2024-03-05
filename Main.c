#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>  
#include <sys/types.h>  
#include <sys/wait.h>  
#include <errno.h>
#include <ctype.h>
#include "Alias.h"

//Used to colour the output of the terminal
#define setTerminalBlue "\x1b[34m"
#define resetTerminalColour "\x1b[0m"

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

void forkAndExec(char* argv[51]);
int internalCommand(char* argv[51]);
void getPath();
void setPath(char* pathString);
void changeDirectory(char* argv[51]);
char* delimiters=" \t<>|;&\n";
void addToHistory(char *command);
void displayHistory();
void getCommandFromHistory(char* command[51]);
void Tokeniser(char *command);
int getnum(int startPos, char str[51]);
void saveHistory(CommandHistory history[20], int historyCount);
void loadHistory(int historyCount);
void trim(char *str);


int main(){
    char* const savedPath = getenv("PATH");

    chdir(getenv("HOME"));

    while(1){
        //Gets the current working directory 
        char currentDir[150];
        getcwd(currentDir,150);

        //below is LOADING FUNCTION
        void loadHistory(int historyCount);

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
           
            
            saveHistory(history, historyCount);

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
        addAlias(argv[1], argv[2], aliasList);
        return 1;
    }
    else if (strcmp(argv[0],"unalias")==0){
        removeAlias(argv[1], aliasList);
        return 1;
    }
    
    else{
        for(int i = 0; i < 20; i++){
            if(strcmp(aliasList[i].alias, argv[0]) == 0){
                Tokeniser(aliasList[i].command);
                return 1;
            }
        }
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

void addToHistory(char* command){
if(command[0] != '!') {
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
}

void displayHistory(){
    // goes through all the current commands and displays them
    for(int i = 0; i < currentCommandNo; i++ ){
        printf("%d: %s\n", (i+1), history[i].commandLine);
    }
}

void getCommandFromHistory(char* command[51]) {

int num;

if (strcmp(command[0], "!!") == 0) {
        if (historyCount > 0) {
            
            Tokeniser(history[historyCount - 1].commandLine); 
            return;
        } else {
            printf("Error: No commands in history\n");
            return;
        }
}
else if (command[0][1] == 45) {
    num = getnum(2, command[0]);
    if (num <= 0 || num > historyCount) {
        printf("Error: Invalid history index\n");
        return;
    }
    
    Tokeniser(history[historyCount - num].commandLine);

}
else {
    num = getnum(1, command[0]);
    if (num <= 0 || num > historyCount) {
        printf("Error: Invalid history index\n");
        return;
    }
    
    Tokeniser(history[num - 1].commandLine); 
    }
        
    return; 
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

void saveHistory(CommandHistory history[20], int historyCount){
    FILE *f;
    f=fopen(".hist_list.txt", "w");
    if(f==NULL){
        perror("No file");
    }
    else{
        for(int i=0; i < historyCount; i++){
            fprintf(f, "%d\n", history[i].commandNumber);
            fprintf(f, "%s", history[i].commandLine);
    }
    fclose(f);
    }
}

void loadHistory(int historyCount){
 FILE *f;
    f=fopen(".hist_list.txt", "r");
    if(f==NULL){
        perror("Error Loading History");
    }
    else{
        for(int i=0; i < historyCount; i++){
            fscanf(f, "%d\n", &history[i].commandNumber);
            fgets(history[i].commandLine, 512, f);
        }
        fclose(f);
    }
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

 