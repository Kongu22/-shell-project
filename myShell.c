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

int main() {
    welcome(); // Display the welcome banner
    getLocation(); // Display location, username, and hostname information

    return 0;
}
