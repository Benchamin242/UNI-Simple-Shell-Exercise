//struct to map alias to alias 
typedef struct{
    char alias[50];
    char command[50];
} Alias;

void printAliases();
void addAlias(char* argv[51]);
void removeAlias(char* alias);
int tryAlias(char* argv[51]);
void saveAlias();
void loadAlias();