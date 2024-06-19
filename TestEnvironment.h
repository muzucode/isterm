#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>

#include <string.h>
typedef struct {
    char* label; 
    char* projectRoot;
} TestEnvironment;

void testEnvironmentToString(TestEnvironment* testEnvironment) {
    printf("Label: %s\n", testEnvironment->label);
    printf("Project Root: %s\n", testEnvironment->projectRoot);
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

void promptTestEnvironmentCreation() {

}

void readTestEnvironmentsFromConfig() {
    const char* environmentsFile   = "environments.toml";
    FILE* file;
    char buf[512];
    char tomlKeyHeaderBuf[64];
    char* environmentKey;
    TestEnvironment te;
    bool insideEnvConfigBlock = false;


    file = fopen(environmentsFile, "r");

    fseek(file, 0, SEEK_END);
    int fileSize = ftell(file);
    fseek(file, 0, SEEK_SET);

    
    // Parse TOML
    while(fgets(buf, sizeof(buf), file)) {
        // If reading a line that is one character, just a line break... 
        // mark as no longer inside envConfigBlock and re-loop
        if(strlen(buf) == 1 && (strcmp(buf, "\n") == 0)) {
            insideEnvConfigBlock = false;
            continue;
        }
        buf[strcspn(buf, "\n")] = '\0'; // null-term the read line
        // Save the toml key if there is one
        if(buf[0] == '[') {
            insideEnvConfigBlock = true;
            int count = 1;
            tomlKeyHeaderBuf[0] = buf[count];
            while(buf[count+1] != ']') {
                tomlKeyHeaderBuf[count] = buf[count+1];
                count++;
            }

            // Null term the toml env key
            tomlKeyHeaderBuf[count] = '\0';

            // Save environment key
            te.label = strdup(tomlKeyHeaderBuf);
            printf("Successfully read a toml key: %s\n", te.label);
            free(te.label);
        }


    }


    return;
}