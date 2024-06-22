#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "TestEnvironment.h"

// This shell administers usage of test environments

#define SHELL_PREFIX "isT>"
#define MAX_TOKENS_AMOUNT 16
TestEnvironment* activeTestEnvironment;
TestEnvironmentList* testEnvironmentList;

void getActiveEnvironment() {

}

void startTe() {
    printf("Starting test environment...\n");   
    pid_t p = fork();
    if(p<0) {
        perror("Error forking child process\n");
        exit(1);
    } else if (p == 0) {
        printf("In child process\n");
    } else {
        printf("In parent process\n");
    }
}
char** parseTokens(char* input, int* tokensCount) {

    // Duplicate the input arg so we retain its value despite
    // strtok
    char* inputToParse = strdup(input);
    if(inputToParse == NULL) {
        perror("Error strdup inputToParse");
        return NULL;
    }

    // Initialize tokensCount
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
    while (token != NULL && *tokensCount < MAX_TOKENS_AMOUNT) {
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

    free(inputToParse);

    return tokens;
}

int setActiveTestingEnvironment(char* label) {
    TestEnvironment* teBuf = findTestEnvironmentByLabel(testEnvironmentList, label);
    if(teBuf == NULL) {
        return 1;
    } else {
        activeTestEnvironment = teBuf;
        return 0;
    }

}

void listenForInput() {
    char buf[128];
    char* input = NULL;
    char** tokens;
    int tokensCount;


    activeTestEnvironment->label = strdup("-");
    if (activeTestEnvironment->label == NULL) {
        perror("Error strdup testEnvironment->label");
        exit(1);
    }

    while (1) {
        printf("(%s) %s ", activeTestEnvironment->label, SHELL_PREFIX);

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

        if (tokensCount > 0 && strcmp(tokens[0], "exit") == 0) {
            printf("Seeya!\n");
            for (int i = 0; i < tokensCount; i++) {
                free(tokens[i]);
            }
            free(tokens);
            free(input);
            break;
        } else if (tokensCount > 0 && strcmp(tokens[0], "list") == 0) {
            // TODO: Implement printing list of testenvlist
            printf("TODO: implement\n");
        } else if (tokensCount > 0 && strcmp(tokens[0], "use") == 0) {

            // If a testenv argument is provided...
            if(tokens[1] != NULL) {
                int result = setActiveTestingEnvironment(tokens[1]);
                if(result != 0) {
                    printf("Invalid environment \"%s\". Run \"list\" to see a list of available environments.\n", tokens[1]);
                    continue;
                }
            }
        } else if (tokensCount > 0 && strcmp(tokens[0], "start") == 0) {
            startTe();
            printf("Ran command\n");
        } else if (tokensCount > 0 && strcmp(tokens[0], "stop") == 0) {
            printf("Stopping test environment...\n");
            testEnvironmentStop();
        } else if (tokensCount > 0 && strcmp(tokens[0], "te:add")  == 0 ) {
            printf("Add a test environment, below:\n");
            // promptTestEnvironmentCreation();
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

    free(activeTestEnvironment->label);
}


int main() {
    activeTestEnvironment = (TestEnvironment*)malloc(sizeof(TestEnvironment));
    testEnvironmentList = (TestEnvironmentList*)malloc(sizeof(TestEnvironmentList));
    testEnvironmentList = readTestEnvironmentsFromConfig();
    listenForInput();
    return 0;
}
