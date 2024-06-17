#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

// This shell administers usage of test environments

#define SHELL_PREFIX "SH>"
#define MAX_TOKENS_AMOUNT 16
#define MAX_TOKEN_SIZE 128

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

char** parseTokens(char* input, int* tokensCount) {
    // Start here and think about mallocing, reallocing, etc.
    printf("Received input: %s\n", input);
    char** tokens = (char**)malloc(sizeof(char*) * MAX_TOKENS_AMOUNT);

    // Alloc memory for each tokens argument
    for(int i = 0; i < MAX_TOKENS_AMOUNT; i++) {
        tokens[i] = (char*)malloc(MAX_TOKEN_SIZE);
    } 

    // Begin strtok
    char* token = strtok(input, " ");
    // Capture the first token as an arg
    tokens[*tokensCount] = strdup(token);

    // Loop through remaining tokens
    while( token != NULL ) {
        token = strtok(NULL, " ");
        printf("Made it here\n");

        (*tokensCount)++;
        // Add the token str pointer to the tokens
        tokens[*tokensCount] = strdup(token);
    }

    return tokens;

}

void listenForInput() {
    char buf[128];
    char* input;
    char** tokens;
    TestEnvironment* testEnvironment;
    testEnvironment = (TestEnvironment*)malloc(sizeof(TestEnvironment));
    testEnvironment->label = (char*)malloc(sizeof(buf));

    // Initialize the default test environment
    testEnvironment->label = strdup("-");


    while( strcmp(input, "exit") != 0 ) {
        printf("(%s) %s ", testEnvironment->label, SHELL_PREFIX);

        while ( fgets(buf, sizeof(buf), stdin) ) {

            int tokensCount = 0;

            // Read input from buffer
            buf[strcspn(buf, "\n")] = '\0';
            input = (char*)malloc(strlen(buf) + 1);
            if(input == NULL) {
                perror("Error mallocing input");
            }
            strcpy(input, buf);

            tokens = parseTokens(input, &tokensCount);        
            for(int i = 0; i < tokensCount; i++) {
                printf("Token %d: %s\n", i, tokens[i]);
            }    

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

            // Free the tokens and all their data
            for(int i = 0 ; i < MAX_TOKENS_AMOUNT; i++) {
                printf("Freeing memory for token: %s\n", tokens[i]);
                free(tokens[i]);
            }
            printf("Freeing memory for the entire input pointer: %s\n", input);
            free(tokens);
            free(input);


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

