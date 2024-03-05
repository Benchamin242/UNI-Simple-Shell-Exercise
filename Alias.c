#include <stdio.h>
#include <string.h>
#include "Alias.h"
void addAlias(char* alias, char* command, Alias aliasList[20]){
    for(int i = 0; i < 20; i++){
        if(strcmp(aliasList[i].alias, "") == 0){
            strcpy(aliasList[i].alias, alias);
            strcpy(aliasList[i].command, command);
            return;
        }
    }
    printf("Error: No space for new alias\n");

}

void removeAlias(char* alias, Alias aliasList[20]){
    for(int i = 0; i < 20; i++){
        if(strcmp(aliasList[i].alias, alias) == 0){
            strcpy(aliasList[i].alias, "");
            strcpy(aliasList[i].command, "");
            return;
        }
    }
    printf("Error: No such alias\n");
}