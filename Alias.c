#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>  
#include "Alias.h"
#include "InternalCommands.h"

Alias aliasList[20];
int aliasCount=0;

int checkCycle(char* alias, char* command);

void printAliases(){

    for(int i = 0; i < 20; i++){
        if((strcmp(aliasList[i].alias,"") != 0) && (strcmp(aliasList[i].command,"") != 0)){
            printf("Alias '%s' for command '%s'\n",aliasList[i].alias,aliasList[i].command);
        }
    }

    if(aliasCount==0){
        printf("No aliases created\n");
    }
}

void addAlias(char* argv[51]){
    char* alias = argv[1];
    char* command = argv[2];

    if((alias==NULL) | (command==NULL)){
        printf("alias: Command must have arguments 'alias' and 'command'\n");
        return;
    }

    for(int i = 0; i < 20; i++){
        if(strcmp(aliasList[i].alias, "") == 0){
            //Checks if a cycle is being created
            if(!checkCycle(alias, command)){
                printf("Cycle detected, unable to create alias\n");
                return;
            }
            
            strcpy(aliasList[i].alias, alias);
            strcpy(aliasList[i].command, command);

            //Add the command arguments to the alias
            for(int j = 3; j < 51; j++){
                if(argv[j]!=NULL){
                    strncat(aliasList[i].command, " ", strlen(argv[j]));
                    strncat(aliasList[i].command, argv[j], strlen(argv[j]));
                }
            }
            aliasCount++;

            return;
        }
    }
    printf("alias: Error, No space for new alias\n");

}

void removeAlias(char* alias){
    for(int i = 0; i < aliasCount; i++){
        if(strcmp(aliasList[i].alias, alias) == 0){
            strcpy(aliasList[i].alias, "");
            strcpy(aliasList[i].command, "");
            aliasCount--;
            return;
        }
    }
    printf("Error: No such alias\n");
}

int tryAlias(char* argv[51]){
    for(int i = 0; i < 20; i++){
        if(strcmp(aliasList[i].alias, argv[0]) == 0){
            char command[512];
            strcpy(command, aliasList[i].command);
            for(int j = 1; j < 51; j++){
                if(argv[j]!=NULL){
                    strncat(command, " ", strlen(argv[j]));
                    strncat(command, argv[j], strlen(argv[j]));
                }
            }
            Tokeniser(command);
            return 1;
        }
    }

    return 0;
}

void saveAlias(){
    char currentDir[150];
    getcwd(currentDir,150);
    chdir(getenv("HOME"));
    FILE *f;
    f=fopen(".aliases.txt", "w");

    for(int i=0; i<aliasCount; i++){
        fprintf(f, "%s,%s\n", aliasList[i].alias,aliasList[i].command);
    }
    fclose(f);
    
    chdir(currentDir);
}

void loadAlias(){
    FILE *f;
    char* aliasFileName = ".aliases.txt";
    f=fopen(aliasFileName, "r");

    //Creates the file if it doesn't exit
    if(f==NULL){
        printf("No alias file found\n");
        return;
    }

    char buffer[512];
    int i=aliasCount;

    while (fgets(buffer, sizeof(buffer), f)) {
        char *token = strtok(buffer, ",");
        int tokenCount = 0;
        while (token) { 
            if(tokenCount==0){
                strcpy(aliasList[i].alias, token);
            }
            else{
                strcpy(aliasList[i].command, token);
            }
            token = strtok(NULL, ",");
            tokenCount++;
        }
        char lastChar = aliasList[i].command[strlen(aliasList[i].command)-1];
        if(lastChar == '\n'){
            aliasList[i].command[strlen(aliasList[i].command)-1] = '\0';
        }
        i++;
    }
    aliasCount=i;
    fclose(f);
}

int checkCycle(char* alias, char* command){
    for(int i = 0; i < 20; i++){
        if(strcmp(command, aliasList[i].alias) == 0){
            if(strcmp(alias, aliasList[i].command) == 0){
                return 0;
            }
            return checkCycle(alias, aliasList[i].command);
        }
    }
    return 1;
}