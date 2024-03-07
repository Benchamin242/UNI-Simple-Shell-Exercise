#include <stdio.h>
#include <string.h>
#include "Alias.h"
#include "InternalCommands.h"

Alias aliasList[20];

void printAliases(){
    int aliasCount = 0;

    for(int i = 0; i < 20; i++){
        if((strcmp(aliasList[i].alias,"") != 0) && (strcmp(aliasList[i].command,"") != 0)){
            printf("Alias '%s' for command '%s'\n",aliasList[i].alias,aliasList[i].command);
            aliasCount++;
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