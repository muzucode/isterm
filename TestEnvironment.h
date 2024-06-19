#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

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
    FILE* file = fopen("environments.toml", "r");
    char buf[512];
    char tomlKeyHeaderBuf[64];

    fseek(file, 0, SEEK_END);
    int fileSize = ftell(file);
    fseek(file, 0, SEEK_SET);

    

    while(fgets(buf, sizeof(buf), file)) {
        buf[strcspn(buf, "\n")] = '\0'; // null-term the read line

        if(buf[0] == '[') {
            int count = 1;
            tomlKeyHeaderBuf[count] = buf[count];
            while(tomlKeyHeaderBuf[count] != ']') {
                tomlKeyHeaderBuf[count] = buf[count];
                printf("Reading char: %c\n", buf[count]);
                count++;
            }
            tomlKeyHeaderBuf[count+1] = '\0';
        }

        printf("Successfully read the toml key: %s\n", tomlKeyHeaderBuf);

    }


    return;
}