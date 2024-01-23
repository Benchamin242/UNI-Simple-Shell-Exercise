#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char* UserPrompt();
char* delimiters=" \t<>|;&\n";

int main(){
    while(1){
        char* input=UserPrompt();
        if (!(strcmp(input,"\n")==0))
        {
            //tokenizes string and stores it
            char commands[50][511];
            char* token=strtok(input, delimiters);
            int i=0;

            while(token!=NULL){
                if((strcmp(token,"\n")!=0)){
                    printf("Command entered: '%s'\n",token);                  
                }
                else{
                    //To keep the output consistent 
                    printf("\n");
                }

                strcpy(commands[i], token);
                i++;
                token=strtok(NULL, delimiters);  
            }
        }
    }
}

//Returns the users' input, or quits if 'exit' is inputted or CTRL+D is detected
char* UserPrompt(){
    char* input=(char *)malloc(512 * sizeof(char));
    printf("\n|-o-| ");
    char* fgetsResult = fgets(input, 511, stdin);

    if((fgetsResult==NULL)|(strcmp(input,"exit\n")==0)){
        printf("\n");
        exit(0);
    }

    return input;
}