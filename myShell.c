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
    char *str = (char *)malloc(size * sizeof(char));
    if (!str) {
        fprintf(stderr, "Memory allocation failed\n");
        return NULL;
    }

    while ((ch = getchar()) != '\n') {
        str[size - 1] = ch;
        size++;
        char *temp = realloc(str, size * sizeof(char));
        if (!temp) {
            fprintf(stderr, "Memory allocation failed\n");
            free(str);
            return NULL;
        }
        str = temp;
    }
    str[size - 1] = '\0';

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

int main() {
    welcome(); // Display the welcome banner
    getLocation(); // Display location, username, and hostname information

    printf("Enter your command: ");
    char *userInput = getInputFromUser(); // Dynamically get input from the user
    if (userInput == NULL) {
        fprintf(stderr, "Failed to read input\n");
        return 1;
    }

    char **splitResults = splitArgument(userInput); // Split the user input into arguments
    if (splitResults == NULL) {
        free(userInput); // Remember to free userInput if splitArgument fails
        return 1;
    }

    printf("Split Arguments:\n");
    for (int i = 0; splitResults[i] != NULL; i++) {
        printf("%s\n", splitResults[i]);
    }

    // Free the allocated memory for splitResults
    for (int i = 0; splitResults[i] != NULL; i++) {
        free(splitResults[i]);
    }
    free(splitResults);
    free(userInput); // Also, free the userInput now that we're done with it

    return 0;
}