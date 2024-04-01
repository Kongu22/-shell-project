#ifndef MYSHELL_H
#define MYSHELL_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#include <limits.h> 
#include <errno.h>
#include <sys/wait.h> 
#include <libgen.h>  
#include <string.h>


#define BUFF_SIZE 1024

void bold();
void blue();
void reset();
void welcome();
void getLocation();
char *getInputFromUser();
void move(char **args);
char **splitArgument(char *str);
void logout(char *input);
void cd(char **args);
void cp(char **args);
void deleteFile(char **args);
void mypipe(char **argv1, char **argv2);
void echoppend(char **args);
void echorite(char **args);
void readFile(char **args);
void processCommand(char *userInput);

#endif 