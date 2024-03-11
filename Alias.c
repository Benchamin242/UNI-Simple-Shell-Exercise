#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>  
#include "Alias.h"
#include "InternalCommands.h"

Alias aliasList[20];
int aliasCount=0;

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
    for(int i = 0; i < 20; i++){
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
            Tokeniser(aliasList[i].command);
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
    if(f==NULL){
        perror("No file");
    }
    else{
        for(int i=0; i<aliasCount; i++){
            fprintf(f, "%s ", aliasList[i].alias);
            fprintf(f, "%s", aliasList[i].command);
        }
        fclose(f);
    }
    chdir(currentDir);
}

void loadAlias(){
    FILE *f;
    f=fopen(".aliases.txt", "r");

    //Creates the file if it doesn't exit
    if(f==NULL){
        f=fopen(".aliases.txt", "w");
        fclose(f);
        f=fopen(".aliases.txt", "r");
    }

    char buffer[512];
    int i=aliasCount;
    while(fgets(buffer, sizeof(buffer), f)!= NULL){
        strcpy(aliasList[i].alias, buffer);
        fgets(buffer, sizeof(buffer), f);
        strcpy(aliasList[i].command, buffer);
        i++;
    }
    aliasCount=i;
    fclose(f);
}
