#include "History.h"
#include "InternalCommands.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>  
#include <stdlib.h>

int historyCount = 0;

int currentCommandNo = 0;

CommandHistory history[20];

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

void addToHistory(char* command){
    if(command[0] != '!') {
        if(historyCount < 20){
            currentCommandNo++;
            history[historyCount].commandNumber = currentCommandNo;
            strcpy(history[historyCount].commandLine, command);
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
        }
    }
}

void displayHistory(){
    //Goes through all the current commands and displays them
    for(int i = 0; i < currentCommandNo; i++ ){
        printf("%d: %s", (i+1), history[i].commandLine);
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

void saveHistory(){
    //Gets the current working directory 
    char currentDir[150];
    getcwd(currentDir,150);
    chdir(getenv("HOME"));
    FILE *f;
    f=fopen(".hist_list.txt", "w");
    if(f==NULL){
        perror("No file");
    }
    else{
        for(int i=0; i < historyCount; i++){
            fprintf(f, "%s", history[i].commandLine);
        }
        fclose(f);
    }
    chdir(currentDir);
}


void loadHistory(){
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