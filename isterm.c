#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

// This shell administers usage of test environments

#define SHELL_PREFIX "SH>"
#define MAX_TOKENS_AMOUNT 16

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
    // Initialize tokensCount
    char* inputToParse = strdup(input);
    *tokensCount = 0;

    // Allocate memory for tokens array
    char** tokens = (char**)malloc(MAX_TOKENS_AMOUNT * sizeof(char*));
    if (tokens == NULL) {
        perror("Error mallocing tokens in parseTokens");
        return NULL;
    }

    // Begin strtok
    char* token = strtok(inputToParse, " ");

    // Loop through all tokens
    while (token != NULL && *tokensCount < 16) {
        tokens[*tokensCount] = strdup(token);
        if (tokens[*tokensCount] == NULL) {
            perror("Error strdup a token");
            for (int i = 0; i < *tokensCount; i++) {
                free(tokens[i]);
            }
            free(tokens);
            return NULL;
        }
        (*tokensCount)++;
        token = strtok(NULL, " ");
    }

    return tokens;
}

void listenForInput() {
    char buf[128];
    char* input = NULL;
    char** tokens;
    int tokensCount;
    TestEnvironment* testEnvironment;

    // Allocate and initialize the default test environment
    testEnvironment = (TestEnvironment*)malloc(sizeof(TestEnvironment));
    if (testEnvironment == NULL) {
        perror("Error mallocing testEnvironment");
        exit(1);
    }
    testEnvironment->label = strdup("-");
    if (testEnvironment->label == NULL) {
        perror("Error strdup testEnvironment->label");
        free(testEnvironment);
        exit(1);
    }

    while (1) {
        printf("(%s) %s ", testEnvironment->label, SHELL_PREFIX);

        if (fgets(buf, sizeof(buf), stdin) == NULL) {
            perror("Error reading input");
            break;
        }

        buf[strcspn(buf, "\n")] = '\0';  // Remove newline character

        input = strdup(buf);
        if (input == NULL) {
            perror("Error strdup input");
            break;
        }

        tokens = parseTokens(input, &tokensCount);
        if (tokens == NULL) {
            printf("Tokens is NULL for some reason... restarting loop\n");
            free(input);
            continue;
        }

        for (int i = 0; i < tokensCount; i++) {
            printf("Token %d: %s\n", i, tokens[i]);
        }

        if (tokensCount > 0 && strcmp(tokens[0], "exit") == 0) {
            printf("Seeya!\n");
            for (int i = 0; i < tokensCount; i++) {
                free(tokens[i]);
            }
            free(tokens);
            free(input);
            break;
        } else if (tokensCount > 0 && strcmp(tokens[0], "te:list") == 0) {
            printf("Available test environments: \n");
        } else if (tokensCount > 0 && strcmp(tokens[0], "te:use") == 0) {
            free(testEnvironment->label);
            testEnvironment->label = strdup("default");
        } else if (tokensCount > 0 && strcmp(tokens[0], "te:start") == 0) {
            printf("Starting test environment...\n");
        } else if (tokensCount > 0 && strcmp(tokens[0], "te:stop") == 0) {
            printf("Stopping test environment...\n");
            testEnvironmentStop();
        } else {
            printf("Unrecognized command \"%s\"\n", input);
        }

        // Free the tokens and all their data
        for (int i = 0; i < tokensCount; i++) {
            free(tokens[i]);
        }
        free(tokens);
        free(input);
    }

    free(testEnvironment->label);
    free(testEnvironment);
}

void startServer() {

}

int main() {
    listenForInput();
    return 0;
}
