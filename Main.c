#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char* UserPrompt();

int main(){
    while(1){
        char* input = UserPrompt();

        if (!(strcmp(input,"\n")==0))
        {
            printf("Command entered: %s",input);
        }
    }
}

//Returns the users' input, or quits if 'exit' is inputted or CTRL+D is detected
char* UserPrompt(){
    char* input = (char*)malloc(511);
    printf("\n|-o-| ");
    char* fgetsResult = fgets(input, sizeof(input), stdin);

    if((fgetsResult==NULL)|(strcmp(input,"exit\n")==0)){
        printf("\n");
        exit(0);
    }

    return input;
}