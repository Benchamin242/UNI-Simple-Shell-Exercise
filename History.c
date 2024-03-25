#include "History.h"
#include "InternalCommands.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>  
#include <stdlib.h>

int historyCount = 0;

int currentCommandNo = 0;

CommandHistory history[20];

int getHistoryNumber(int startPos, char str[51]){
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

void addToHistory(char* command){
    if(command[0] != '!') {
        if(historyCount < 20){
            currentCommandNo++;
            history[historyCount].commandNumber = currentCommandNo;
            strcpy(history[historyCount].commandLine, command);
            char lastChar = history[historyCount].commandLine[strlen(history[historyCount].commandLine)-1];
            if(lastChar != '\n'){
                history[historyCount].commandLine[strlen(history[historyCount].commandLine)] = '\n';
            }
            historyCount++;
        }
        else{
            // loops through array moving each history member forward 
            for(int i = 1; i<20 ; i++){
                history[i - 1].commandNumber = history[i].commandNumber;
                strcpy(history[i - 1].commandLine, history[i].commandLine);
            }
            history[19].commandNumber = currentCommandNo;
            strcpy(history[19].commandLine, command);
            char lastChar = history[19].commandLine[strlen(history[19].commandLine)-1];
            if(lastChar != '\n'){
                history[19].commandLine[strlen(history[19].commandLine)] = '\n';
            }
        }
    }
}

void displayHistory(){
    //Goes through all the current commands and displays them
    for(int i = 0; i < currentCommandNo-1; i++ ){
        printf("%2d  %s", (i+1), history[i].commandLine);
    }
}

void getCommandFromHistory(char* command[51]) {

    int num;

    if (strcmp(command[0], "!!") == 0) {
        if (historyCount > 0) {
            char command[51];
            strcpy(command,history[historyCount - 1].commandLine);  
            addToHistory(command);          
            Tokeniser(command); 
            return;
        } else {
            printf("Error: No commands in history\n");
            return;
        }
    }
    else if (command[0][1] == '-') {
        num = getHistoryNumber(2, command[0]);
        if (num <= 0 || num > historyCount) {
            printf("Error: Invalid history index\n");
            return;
        }
        char command[51];
        strcpy(command,history[historyCount-num-1].commandLine);
        addToHistory(command);
        Tokeniser(command);

    }
    else {
        num = getHistoryNumber(1, command[0]);
        if (num <= 0 || num > historyCount) {
            printf("Error: Invalid history index\n");
            return;
        }
        char command[51];
        strcpy(command,history[num - 1].commandLine);
        addToHistory(command);
        Tokeniser(command); 
    }
        
    return; 
}

void saveHistory(){
    //Gets the current working directory 
    char currentDir[150];
    getcwd(currentDir,150);
    chdir(getenv("HOME"));
    FILE *f;
    char* historyFileName = ".hist_list";
    f = fopen(historyFileName, "w");
    for(int i=0; i < 20; i++){
        fprintf(f, "%s", history[i].commandLine);
    }
    fclose(f);
    
    chdir(currentDir);
}

void loadHistory(){
    //Gets the current working directory 
    char currentDir[150];
    getcwd(currentDir,150);
    chdir(getenv("HOME"));

    FILE *f;
    f=fopen(".hist_list", "r");

    if(f==NULL){
        printf("No history file found\n");
        return;
    }

    for(int i=0; i < 20; i++){
        char line[512] = "";
        fgets(line, 512, f);
        if(strcmp(line,"")==0){
            fclose(f);
            return;
        }
        strcpy(history[i].commandLine,line);
        history[i].commandNumber = i+1;
        currentCommandNo++;
        historyCount = i;
    }

    fclose(f);

    chdir(currentDir);
}