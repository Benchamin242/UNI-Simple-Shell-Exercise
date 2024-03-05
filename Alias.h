#ifndef LIBRARY_H
#define LIBRARY_H
//struct to map alias to alias 
typedef struct{
    char alias[50];
    char command[50];
} Alias;

Alias aliasList[20];


void addAlias(char* alias, char* command, Alias aliasList[20]);
void removeAlias(char* alias, Alias aliasList[20]);

#endif