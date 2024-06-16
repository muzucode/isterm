#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

// This shell administers usage of test environments

#define SHELL_PREFIX "SH>"

typedef struct {
    char* label;
    char* startCmd;
    char* entryPoint;
} TestEnvironment;

void getActiveEnvironment() {

}

void testEnvironmentStart() {

}

void testEnvironmentStop() {

}

void printListTestEnvironments() {

}

void parseArgs() {
    // Implement strtok here
}

void listenForInput() {
    char buf[128];
    char* input;
    TestEnvironment* testEnvironment;
    testEnvironment = (TestEnvironment*)malloc(sizeof(TestEnvironment));
    testEnvironment->label = (char*)malloc(sizeof(char));

    // Initialize the default test environment
    testEnvironment->label = strdup("-");

    while( strcmp(input, "exit") != 0 ) {
        printf("(%s) %s ", testEnvironment->label, SHELL_PREFIX);

        while ( fgets(buf, sizeof(buf), stdin) ) {

            // Read input from buffer
            buf[strcspn(buf, "\n")] = '\0';
            input = (char*)malloc(strlen(buf) + 1);
            strcpy(input, buf);

            if(strcmp(input, "exit") == 0) {
                printf("%s", "Seeya!\n");
                break;
            }
            // Use the correct command
            else if (strcmp(input, "te:list") == 0) {
                printf("Available test environments: \n");
            }
            else if(strcmp(input, "te:use") == 0) {
                testEnvironment->label = strdup("default");
            }            
            else if(strcmp(input, "te:start") == 0) {
                printf("Starting test environment...\n");
            }
            else if(strcmp(input, "te:stop") == 0) {
                printf("Stopping test environment...\n");
                testEnvironmentStop();
            } else {
                printf("Unrecognized command \"%s\"\n", input);
            }

            break;
        }

    }

}


void startServer() {

}

int main() {


    listenForInput();

    return 0;
}

