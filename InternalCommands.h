void forkAndExec(char* argv[51]);
int internalCommand(char* argv[51]);
void getPath();
void setPath(char* pathString);
void changeDirectory(char* argv[51]);
void Tokeniser(char *command);
int getnum(int startPos, char str[51]);
void trim(char *str);