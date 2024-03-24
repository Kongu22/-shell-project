#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#include <limits.h> // For PATH_MAX
#include <errno.h>

#define BUFF_SIZE PATH_MAX

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

void welcome() {
    printf("||=============================||\n");
    printf("||           __/o \\_           ||  Welcome to Daniel's shell\n");
    printf("||           \\____  \\          ||  Version 1.0\n");
    printf("||               /   \\         ||\n");
    printf("||         __   //\\   \\        ||\n");
    printf("||      __/o \\-//--\\   \\_/     ||  https://github.com/<user>\n");
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
    str[index] = '\0'; // Null-terminate the string

    // Check if the input is "STOP"
    if (index == 4 && str[0] == 'S' && str[1] == 'T' && str[2] == 'O' && str[3] == 'P') {
        free(str);
        return NULL;
    }

    return str;
}

// The splitArgument function as requested
char **splitArgument(char *str) {
    int size = 1; // Start with size 1 for the NULL terminator at the end
    char **arguments = (char **)malloc(size * sizeof(char *));
    if (arguments == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        return NULL;
    }

    int start = 0; // Start of a word
    int length = 0; // Length of a word
    for (int i = 0; str[i] != '\0'; ++i) {
        if (str[i] != ' ') {
            ++length; // Increase word length if current character is not a space
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

void logout(char *input) {
    char command[10]; // Assuming "exit" won't be longer than 9 characters + '\0'
    char rest[100]; // Assuming the rest of the input won't exceed 99 characters + '\0'

    // Attempt to read the command "exit" from the input, ignoring spaces before and after
    if (sscanf(input, " %9s %[^\n]", command, rest) == 1) {
        // Check if the command is "exit" without using strcmp
        if (command[0] == 'e' && command[1] == 'x' && command[2] == 'i' && command[3] == 't' && command[4] == '\0') {
            free(input); // Free the memory
            puts("Logging out..."); // Print the logout message
            sleep(1); // Delay for 1 second
            puts("."); // Print the logout message
            sleep(1); // Delay for 1 second
            puts(".."); // Print the logout message
            sleep(1); // Delay for 1 second
            puts("..."); // Print the logout message
            sleep(1); // Delay for 1 second
            puts("Logged out successfully!"); // Print the logout message
            sleep(1); // Delay for 1 second
            puts("Have a good day :)"); // Print the logout message
            exit(EXIT_SUCCESS); // Exit the program successfully
        }
    }
    // If we reached here, the input is not recognized as the "exit" command with extra characters
    // printf("Command not recognized: %s\n", input);
}

void cd(char **args) {
    if (args[1] == NULL) {
        fprintf(stderr, "-myShell: cd: missing argument\n");
        return;
    }

    if (args[2] == NULL) { // Single argument path, directly use args[1]
        if (chdir(args[1]) != 0) {
            fprintf(stderr, "-myShell: cd: %s: No such file or directory\n", args[1]);
        }
        return; // Early return to avoid further processing
    }

    // If we reach here, it means there are multiple arguments potentially with quotes
    char *path = (char *)malloc(BUFF_SIZE * sizeof(char));
    if (path == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        return;
    }
    int pathIndex = 0; // Initialize the path index

    for (int i = 1; args[i] != NULL; i++) {
        char *part = args[i];
        int startIndex = (i == 1 && part[0] == '\"') ? 1 : 0;
        int endIndex = startIndex; // Initialize endIndex with startIndex

        while (part[endIndex] != '\0' && !(i > 1 && part[endIndex] == '\"')) {
            endIndex++;
        }

        // Copy characters from part to path
        for (int j = startIndex; j < endIndex; j++) {
            path[pathIndex++] = part[j];
        }

        // If the part ends with a closing quote, stop processing further arguments
        if (part[endIndex] == '\"') {
            break;
        }

        // Add a space between parts, but not after the last part
        if (args[i + 1] != NULL) {
            path[pathIndex++] = ' ';
        }
    }
    path[pathIndex] = '\0'; // Null-terminate the concatenated path

    if (chdir(path) != 0) {
        fprintf(stderr, "-myShell: cd: %s: No such file or directory\n", path);
    }

    free(path); // Free allocated memory
}

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

    while ((ch = fgetc(src)) != EOF)
    {
        fputc(ch, dest);
    }

    fclose(src);
    fclose(dest);
    printf("File copied successfully from %s to %s\n", srcPath, destPath);
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

        logout(userInput); // Check for logout command

        char **splitResults = splitArgument(userInput);
        if (splitResults != NULL) {
            // Handling for the cd command
            if (splitResults[0] != NULL && splitResults[0][0] == 'c' && splitResults[0][1] == 'd' && splitResults[0][2] == '\0') {
                cd(splitResults);
                getLocation(); // Update and display the current location
            }
            // Handling for the cp command
            else if (splitResults[0] != NULL && splitResults[0][0] == 'c' && splitResults[0][1] == 'p' && splitResults[0][2] == '\0') {
                if (splitResults[1] == NULL || splitResults[2] == NULL) {
                    printf("Error: cp requires two arguments, the source and destination paths.\n");
                } else {
                    cp(splitResults);
                }
            }

            // Free the allocated memory for splitResults
            for (int i = 0; splitResults[i] != NULL; i++) {
                free(splitResults[i]);
            }
            free(splitResults);
        }

        free(userInput); // Free the userInput now that we're done with it
    }

    return 0;
}