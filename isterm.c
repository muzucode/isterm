#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

// This shell administers usage of test environments

#define SHELL_PREFIX "isT>"
#define MAX_TOKENS_AMOUNT 16

typedef struct {
    char* label;
    char* startCmd;
    char* entryPoint;
} TestEnvironment;

void getActiveEnvironment() {

}

int testEnvironmentStart() {
    pid_t pid;
    pid = fork();

    if(pid < 0) {
        perror("Error forking child process\n");
    } else if (pid == 0) {

        char* dockerComposeFilePath = "/Users/sean/Documents/Coding/Merkbench/testing-environments/docker-instances/docker-compose.yml";
        char* swarmInitArgs[4] = {"docker","swarm", "init", NULL};
        execvp(swarmInitArgs[0], swarmInitArgs);

        perror("execvp failed\n");
        return 1;

        char* stackDeployArgs[6] = {"docker","stack", "deploy", "-c", dockerComposeFilePath, NULL};
        execvp(stackDeployArgs[0], stackDeployArgs);
    } else {
        // Parent process
        int status;
        waitpid(pid, &status, 0);  // Wait for the child process to complete
    }

    return 0;

}

int testEnvironmentStop() {

    pid_t p;

    p = fork();
    if(p < 0) {
        perror("Error forking child process");
        return 1;
    } else if (p == 0) {
        char* args[5] = {"docker", "swarm", "leave", "--force", NULL};
        execvp(args[0], args);
        
        perror("execvp didn't terminate isterm, signaling an error\n");
        return 1;
    } else {
        int status;
        waitpid(p, &status, 0);
    }

    return 0;
}

void printListTestEnvironments() {

}

char** parseTokens(char* input, int* tokensCount) {

    // Duplicate the input arg so we retain its value despite
    // strtok
    char* inputToParse = strdup(input);

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

void setActiveTestingEnvironment(char* environmentName) {

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

        if (tokensCount > 0 && strcmp(tokens[0], "exit") == 0) {
            printf("Seeya!\n");
            for (int i = 0; i < tokensCount; i++) {
                free(tokens[i]);
            }
            free(tokens);
            free(input);
            break;
        } else if (tokensCount > 0 && strcmp(tokens[0], "te:list") == 0) {
            printf("default\n");
            printf("seoul-network\n");
        } else if (tokensCount > 0 && strcmp(tokens[0], "te:use") == 0) {
            free(testEnvironment->label);
            if(tokens[1] != NULL) {
                testEnvironment->label = strdup(tokens[1]);
                setActiveTestingEnvironment(tokens[1]);
            } else {
                char* defaultEnvironment = "d";
                testEnvironment->label = strdup(defaultEnvironment);
                setActiveTestingEnvironment(defaultEnvironment);
            }

        } else if (tokensCount > 0 && strcmp(tokens[0], "te:start") == 0) {
            printf("Starting test environment...\n");
            testEnvironmentStart();
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
