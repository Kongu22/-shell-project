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
#include "myShell.h"



// Function prototypes for styling the output
void bold() {
    printf("\033[1m");
}

void blue() {
    printf("\033[34m");
}

void reset() {
    printf("\033[0m");
}
// Function to display the welcome banner
void welcome() {
    printf("||=============================||\n");
    printf("||           __/o \\_           ||  Welcome to Daniel's shell\n");
    printf("||           \\____  \\          ||  Version 1.0\n");
    printf("||               /   \\         ||\n");
    printf("||         __   //\\   \\        ||\n");
    printf("||      __/o \\-//--\\   \\_/     ||  https://github.com/Kongu22\n");
    printf("||      \\____  ___  \\  |       ||\n");
    printf("||           ||   \\ |\\ |       ||  Happy crying!\n");
    printf("||          _||   _||_||       ||\n");
    printf("||=============================||  POSO C))\n");
}

void getLocation() {
    char location[BUFF_SIZE];

    // Get current working directory
    if (getcwd(location, BUFF_SIZE) == NULL) {
        perror("Error getting current working directory");
        return;
    }

    // Get username
    uid_t uid = geteuid();
    struct passwd *pw = getpwuid(uid);
    if (pw == NULL) {
        perror("Error getting username");
        return;
    }

    // Get hostname
    char hostname[BUFF_SIZE];
    if (gethostname(hostname, sizeof(hostname)) != 0) {
        perror("Error getting hostname");
        return;
    }
    
    // Print additional information
    printf("\n");
    printf("\033[1;31mPath:\033[0m \033[32m%s\n\033[0m", location); // Print current path
    printf("\033[1;31mUser:\033[0m \033[32m%s\n\033[0m", pw->pw_name); // Print username
    printf("\033[1;31mHost:\033[0m \033[32m%s\n\033[0m", hostname); // Print hostname
    printf("\n");
}

char *getInputFromUser() {
    char ch;
    int size = 1;
    int index = 0;
    char *str = (char *)malloc(size * sizeof(char));
    if (str == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        return NULL;
    }

    while ((ch = getchar()) != '\n') {
        if (index == size - 1) { // Check if more space is needed
            size *= 2; // Double the size
            char *temp = realloc(str, size * sizeof(char));
            if (temp == NULL) {
                fprintf(stderr, "Memory allocation failed\n");
                free(str);
                return NULL;
            }
            str = temp;
        }
        str[index++] = ch;
    }
    str[index] = '\0'; 

    // Check if the input is "STOP"
    if (index == 4 && str[0] == 'S' && str[1] == 'T' && str[2] == 'O' && str[3] == 'P') {
        free(str);
        return NULL;
    }

    return str;
}

void move(char **args) {
    if (args[1] == NULL || args[2] == NULL) {
        printf("Error: Missing source or destination path.\n");
        return;
    }

    char *srcPath = args[1];
    char *destPath = args[2];
    char newPath[BUFF_SIZE];

    // Manually check if destPath is ".."
    if (destPath[0] == '.' && destPath[1] == '.') {
        // Moving back to parent directory
        char *base = basename(srcPath); // Get the basename of the source path
        snprintf(newPath, sizeof(newPath), "%s/%s", destPath, base);
    } else {
        // Normal move operation
        snprintf(newPath, sizeof(newPath), "%s/%s", destPath, basename(srcPath));
    }

    if (rename(srcPath, newPath) == 0) {
        printf("File moved successfully to %s\n", newPath);
    } else {
        perror("Ошибка при перемещении файла");
        // Важно здесь завершить выполнение функции, чтобы избежать вывода последующего сообщения.
        return;
    }

}

// Function to split the input string into arguments
char **splitArgument(char *str) {
    int size = 1; 
    char **arguments = (char **)malloc(size * sizeof(char *));
    if (arguments == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        return NULL;
    }

    int start = 0; // Start of a word
    int length = 0; // Length of a word
    for (int i = 0; str[i] != '\0'; ++i) {
        if (str[i] != ' ') {
            ++length; 
        } else if (length > 0) { // End of a word found
            char *word = (char *)malloc((length + 1) * sizeof(char)); // Allocate memory for the word
            if (word == NULL) {
                fprintf(stderr, "Memory allocation failed\n");
                while (--size > 0) {
                    free(arguments[size - 1]);
                }
                free(arguments);
                return NULL;
            }
            for (int j = 0; j < length; j++) {
                word[j] = str[start + j];
            }
            word[length] = '\0'; // Null-terminate the word
            arguments[size - 1] = word;
            size++;
            arguments = (char **)realloc(arguments, size * sizeof(char *));
            if (arguments == NULL) {
                fprintf(stderr, "Memory allocation failed\n");
                free(word);
                while (--size > 0) {
                    free(arguments[size - 1]);
                }
                return NULL;
            }
            length = 0;
            start = i + 1;
        } else {
            start++;
        }
    }

    if (length > 0) {
        char *word = (char *)malloc((length + 1) * sizeof(char));
        if (word == NULL) {
            fprintf(stderr, "Memory allocation failed\n");
            while (--size > 0) {
                free(arguments[size - 1]);
            }
            free(arguments);
            return NULL;
        }
        for (int j = 0; j < length; j++) {
            word[j] = str[start + j];
        }
        word[length] = '\0';
        arguments[size - 1] = word;
        size++;
        arguments = (char **)realloc(arguments, size * sizeof(char *));
        if (arguments == NULL) {
            fprintf(stderr, "Memory allocation failed\n");
            free(word);
            while (--size > 0) {
                free(arguments[size - 1]);
            }
            return NULL;
        }
    }

    arguments[size - 1] = NULL; // Set the last element to NULL

    return arguments;
}

// Function to logout the user if the input is "exit"
void logout(char *input) {
    char command[10];
    char rest[100];

    if (sscanf(input, " %9s %[^\n]", command, rest) == 1) {
        if (command[0] == 'e' && command[1] == 'x' && command[2] == 'i' && command[3] == 't' && command[4] == '\0') {
            free(input);
            puts("Logging out...");
            sleep(1);
            puts(".");
            sleep(1);
            puts("..");
            sleep(1);
            puts("...");
            sleep(1);
            puts("Logged out successfully!");
            sleep(1);
            puts("Have a good day :)");
            exit(EXIT_SUCCESS);
        }
    }
}

// Function to change directory using the "cd" command
void cd(char **args) {
    if (args[1] == NULL) {
        fprintf(stderr, "-myShell: cd: missing argument\n");
        return;
    }
// Check if the path is enclosed in double quotes
    if (args[2] == NULL) {
        if (chdir(args[1]) != 0) {
            fprintf(stderr, "-myShell: cd: %s: No such file or directory\n", args[1]);
        }
        return;
    }
    char *path = (char *)malloc(BUFF_SIZE * sizeof(char));
    if (path == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        return;
    }
    int pathIndex = 0;
// Concatenate the path parts
    for (int i = 1; args[i] != NULL; i++) {
        char *part = args[i];
        int startIndex = (i == 1 && part[0] == '\"') ? 1 : 0;
        int endIndex = startIndex;

        while (part[endIndex] != '\0' && !(i > 1 && part[endIndex] == '\"')) {
            endIndex++;
        }
// Copy the part to the path
        for (int j = startIndex; j < endIndex; j++) {
            path[pathIndex++] = part[j];
        }
// Check if the path is enclosed in double quotes
        if (part[endIndex] == '\"') {
            break;
        }
// Add a space between parts
        if (args[i + 1] != NULL) {
            path[pathIndex++] = ' ';
        }
    }
    path[pathIndex] = '\0';
// Change the directory
    if (chdir(path) != 0) {
        fprintf(stderr, "-myShell: cd: %s: No such file or directory\n", path);
    }

    free(path);
}

// Function to copy a file using the "cp" command
void cp(char **args)
{
    if (args[1] == NULL || args[2] == NULL) {
        printf("Error: Missing source or destination path.\n");
        return;
    }

    char *srcPath = args[1];
    char *destPath = args[2];
    FILE *src, *dest;
    char ch;
// Open the source file
    src = fopen(srcPath, "r");
    if (src == NULL)
    {
        perror("Error opening source file");
        return;
    }
    dest = fopen(destPath, "w");
    if (dest == NULL)
    {
        perror("Error opening destination file");
        fclose(src);
        return;
    }
// Copy the contents of the source file to the destination file
    while ((ch = fgetc(src)) != EOF)
    {
        fputc(ch, dest);
    }

    fclose(src);
    fclose(dest);
    printf("File copied successfully from %s to %s\n", srcPath, destPath);
}
// Function to delete a file using the "delete" command
void deleteFile(char **args) {
    if (args[1] == NULL) {
        printf("Error: Missing file path to delete.\n");
        return;
    }
// Delete the file
    char *filePath = args[1];
    if (unlink(filePath) != 0) {
        perror("Error deleting file");
    } else {
        printf("File deleted successfully: %s\n", filePath);
    }
}
// Function to create a pipe between two commands
void mypipe(char **argv1, char **argv2) {
    if (argv2 == NULL) {
        // Если argv2 равен NULL, просто выполнить первую команду без создания канала
        if (fork() == 0) {
            execvp(argv1[0], argv1);
            perror("Command execution error");
            exit(EXIT_FAILURE);
        }
    } else {
        int fildes[2];
        if (pipe(fildes) == -1) {
            perror("Error creating pipe");
            return;
        }

        if (fork() == 0) {
            close(fildes[0]);
            dup2(fildes[1], STDOUT_FILENO);
            close(fildes[1]);
            execvp(argv1[0], argv1);
            perror("first Command execution error");
            exit(EXIT_FAILURE);
        }

        if (fork() == 0) {
            close(fildes[1]);
            dup2(fildes[0], STDIN_FILENO);
            close(fildes[0]);
            execvp(argv2[0], argv2);
            perror("seond Command execution error");
            exit(EXIT_FAILURE);
        }

        close(fildes[0]);
        close(fildes[1]);
        wait(NULL);
        wait(NULL);
    }
}
// Function to append a string to a file
void echoppend(char **args) {
    if (args[1] == NULL) {
        printf("Error: Missing argument for echoppend.\n");
        return;
    }

    char *inputString = NULL;
    char *filePath = NULL;
    int i = 0;

//search for >> in the arguments
    for (i = 0; args[i] != NULL; i++) {
        if (args[i][0] == '>' && args[i][1] == '>' && args[i][2] == '\0' && args[i + 1] != NULL) {
            filePath = args[i + 1]; 
            break;
        }
    }

    if (filePath == NULL) {
        printf("Error: Missing file path in arguments.\n");
        return;
    }

    // Calculate the total length for the input string
    size_t totalLength = 0;
    for (int j = 0; j < i; j++) {
        for (int k = 0; args[j][k] != '\0'; k++) {
            totalLength++;
        }
        if (j < i - 1) totalLength += 1; 
    }

    inputString = (char *)malloc(totalLength + 1); 
    if (inputString == NULL) {
        perror("Failed to allocate memory for inputString");
        return;
    }

    // Manually concatenate strings before the delimiter to form the input string
    char *currentPosition = inputString;
    for (int j = 0; j < i; j++) {
        char *word = args[j];
        while (*word) {
            *currentPosition++ = *word++;
        }
        if (j < i - 1) {
            *currentPosition++ = ' ';
        }
    }
    *currentPosition = '\0';

    // Open or create the file and append the input string
    FILE *file = fopen(filePath, "a");
    if (file == NULL) {
        perror("Error opening file");
        free(inputString);
        return;
    }

    fprintf(file, "%s\n", inputString);
    fclose(file);
    free(inputString);
    printf("Appended to file: %s\n", filePath);
}
// replace the content of a file with a string
void echorite(char **args) {
    if (args[1] == NULL) {
        printf("Error: Missing argument for echorite.\n");
        return;
    }

    char *inputString = NULL;
    char *filePath = NULL;
    int i = 0;

    for (i = 0; args[i] != NULL; i++) {
        if (args[i][0] == '>' && args[i + 1] != NULL) {
            filePath = args[i + 1];
            break;
        }
    }

    if (filePath == NULL) {
        printf("Error: Missing file path in arguments.\n");
        return;
    }

    // Calculate the total length for the input string
    size_t totalLength = 0;
    for (int j = 0; j < i; j++) {
        for (int k = 0; args[j][k] != '\0'; k++) {
            totalLength++;
        }
        if (j < i - 1) totalLength++; 
    }

    inputString = (char *)malloc(totalLength + 1);
    if (inputString == NULL) {
        perror("Failed to allocate memory for inputString");
        return;
    }

    char *currentPosition = inputString;
    for (int j = 0; j < i; j++) {
        char *word = args[j];
        while (*word) {
            *currentPosition++ = *word++;
        }
        if (j < i - 1) {
            *currentPosition++ = ' '; 
        }
    }
    *currentPosition = '\0'; 

    // Open or create the file and replace its content with the input string
    FILE *file = fopen(filePath, "w");
    if (file == NULL) {
        perror("Error opening file");
        free(inputString);
        return;
    }

    fprintf(file, "%s\n", inputString); 
    fclose(file);
    free(inputString);
    printf("File content replaced successfully: %s\n", filePath);
}

void readFile(char **args) {
    if (args[0] == NULL) {
        printf("Error: File path not provided.\n");
        return;
    }

    FILE *file = fopen(args[0], "r"); // Attempt to open the file for reading
    if (file == NULL) {
        perror("Error opening file"); // Print an error if the file cannot be opened
        return;
    }

    char buffer[1024]; // Buffer for reading from the file
    while (fgets(buffer, sizeof(buffer), file) != NULL) { // Read each line from the file
        printf("%s", buffer); // Print the line to the console
    }

    fclose(file); // Close the file after reading its contents
}


// Function to process the user's command
void processCommand(char *userInput) {
    char **splitResults = splitArgument(userInput);
    char **argv1 = NULL;
    char **argv2 = NULL;
    int pipeFound = 0;

    for (int i = 0; splitResults && splitResults[i] != NULL; i++) {
        if (splitResults[i][0] == '|' && splitResults[i][1] == '\0') {
            splitResults[i] = NULL;
            argv1 = splitResults;
            argv2 = &splitResults[i + 1];
            pipeFound = 1;
            break;
        }
    }
// Check if a pipe was found
    if (pipeFound) {
        mypipe(argv1, argv2);
    } else {
        if (splitResults[0] != NULL) {
            if (splitResults[0][0] == 'c' && splitResults[0][1] == 'd' && splitResults[0][2] == '\0') {
                cd(splitResults);
                getLocation();
            } else if (splitResults[0][0] == 'c' && splitResults[0][1] == 'p' && splitResults[0][2] == '\0') {
                cp(splitResults);
            } else if (splitResults[0][0] == 'r' && splitResults[0][1] == 'm' && splitResults[0][2] == '\0') {
                deleteFile(splitResults);
            } else if (splitResults[0][0] == 'p' && splitResults[0][1] == 'w' && splitResults[0][2] == 'd' && splitResults[0][3] == '\0') {
                getLocation();
            } else if (splitResults[0][0] == 'm' && splitResults[0][1] == 'v' && splitResults[0][2] == '\0') {
                move(splitResults);
            } else if (splitResults[0][0] == 'e' && splitResults[0][1] == 'c' && splitResults[0][2] == 'h' && splitResults[0][3] == 'o' && splitResults[0][4] == '\0') {
                if (splitResults[1] != NULL && splitResults[2] != NULL) {
                    echoppend(&splitResults[1]);
                } else {
                    fprintf(stderr, "Error: echoppend requires two arguments.\n");
                }
            } else if (splitResults[0][0] == 'e' && splitResults[0][1] == 'r' && splitResults[0][2] == '\0') {
                if (splitResults[1] != NULL && splitResults[2] != NULL) {
                    echorite(&splitResults[1]); 
                } else {
                    fprintf(stderr, "Error: echorite requires arguments.\n");
                }
            }else if (splitResults[0][0] == 'r' && splitResults[0][1] == 'f' && splitResults[0][2] == '\0') {
                if (splitResults[1] != NULL) {
                    readFile(&splitResults[1]); 
                } else {
                    fprintf(stderr, "Error: rf requires a file path as an argument.\n");
                }
                printf("Command not supported or incorrect usage.\n");
            }
        }
    }
    // Free the memory allocated for splitResults
    if (splitResults) {
        for (int i = 0; splitResults[i] != NULL; i++) {
            free(splitResults[i]);
        }
        free(splitResults);
    }
}


int main() {
    welcome(); // Display the welcome banner
    getLocation(); // Display location, username, and hostname information

    while (1) {
        printf("Enter your command: ");
        char *userInput = getInputFromUser(); // Dynamically get input from the user

        if (userInput == NULL) {
            printf("STOP was entered or failed to read input. Exiting...\n");
            break; // Exit the loop if "STOP" was entered or input reading failed
        }

        logout(userInput);// Check if the user wants to logout

        processCommand(userInput); // Process the user's command

        free(userInput); // Free the userInput now that we're done with it
    }

    return 0;
}