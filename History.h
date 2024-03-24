//Command history struct
typedef struct{
    int commandNumber;
    char commandLine[512];
} CommandHistory;

void addToHistory(char *command);
void displayHistory();
void getCommandFromHistory(char* command[51]);
void saveHistory();
void loadHistory();