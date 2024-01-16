#include <stdio.h>

int main(){
    char input[511]="";
    printf("|-o-| ");
    fgets(input, sizeof(input), stdin);
    printf("\nCommand entered: %s",input); 
}
