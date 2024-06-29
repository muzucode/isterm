#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

// TOML File must end in at least two newlines to circumvent issues with
// EOF not being detected

#define MAX_START_COMMANDS 6

typedef struct {
    char* label; 
    char* description;
    char* technology;
    char* projectRoot;
    char* start;
    char* stop;
} TestEnvironment;

typedef struct {
    TestEnvironment *environments;
    size_t size;
    size_t capacity;
} TestEnvironmentList;

// Initialize the list
void initTestEnvironmentList(TestEnvironmentList *list) {
    list->size = 0;
    list->capacity = 10; // Initial capacity
    int totalBytesToAllocForEnvironments = list->capacity * sizeof(TestEnvironment);
    list->environments = (TestEnvironment*)malloc(totalBytesToAllocForEnvironments);
    if(list->environments == NULL) {
        perror("Error mallocing TestEnvironmentList");
        exit(1);
    }
}

// Add a TestEnvironment to the list
void addTestEnvironment(TestEnvironmentList *list, TestEnvironment *env) {
    // Resize the array if needed
    if (list->size >= list->capacity) {
        list->capacity *= 2;
        list->environments = (TestEnvironment*)realloc(list->environments, list->capacity * sizeof(TestEnvironment));
        if(list->environments == NULL) {
            perror("Error reallocing TestEnvironmentList");
            exit(1);
        }
    }
    list->environments[list->size++] = *env;
}


TestEnvironment* findTestEnvironmentByLabel(TestEnvironmentList *list, const char *label) {
    // printf("Size of list: %zu\n", list->size);
    for (size_t i = 0; i < list->size; i++) {
        // printf("Label found: %s\n", list->environments[i].label);
        if (strcmp(list->environments[i].label, label) == 0) {
            return &list->environments[i];  
        }
    }
    printf("Environment not found\n");
    return NULL; // Not found
}

TestEnvironment* getActiveTestEnvironment() {
    // TODO: Implement this, then call this inside of
    // testEnvironmentStart and testEnvironmentStop
    TestEnvironment* te;
    return te;
}

void printTestEnvironment(TestEnvironment* testEnvironment) {
    // TODO: Implement
    printf("Label: %s\n", testEnvironment->label);
    printf("Project Root: %s\n", testEnvironment->projectRoot);
    printf("Description: %s\n", testEnvironment->description);
    printf("Technology: %s\n", testEnvironment->technology);
    printf("Start: %s\n", testEnvironment->start);
    printf("Stop: %s\n", testEnvironment->stop);
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

void setEnvironmentProperty(char* property, char* value, TestEnvironment* te) {
    // printf("Setting property-- %s:%s\n", property, value);
    if(strcmp(property, "description") == 0) {
        te->description = strdup(value);
    }
    if(strcmp(property, "technology") == 0) {
        te->technology = strdup(value);
    }
    if(strcmp(property, "project-root") == 0) {
        te->projectRoot = strdup(value);
    }
    if(strcmp(property, "start") == 0) {
        te->start = strdup(value);
    }    
    if(strcmp(property, "stop") == 0) {
        te->stop = strdup(value);
    }    

    
}

TestEnvironmentList* readTestEnvironmentsFromConfig() {
    const char* environmentsFile   = "environments.toml";
    FILE* file;
    char buf[512];
    char envNameBuf[64];
    char envPropKeyBuf[64];
    char envPropValueBuf[512];
    char* envPropKey;
    char* envPropValue;
    TestEnvironment* te;
    bool insideEnvConfigBlock = false;
    TestEnvironmentList* envList;

    initTestEnvironmentList(envList);


    file = fopen(environmentsFile, "r");

    fseek(file, 0, SEEK_END);
    int fileSize = ftell(file);
    fseek(file, 0, SEEK_SET);

    
    // Parse TOML
    while(fgets(buf, fileSize, file)) {
        // If reading a line that is one character, just a line break... 
        // mark as no longer inside envConfigBlock and re-loop
        if(strlen(buf) == 1 && (strcmp(buf, "\n") == 0)) {
            if(insideEnvConfigBlock) {
                addTestEnvironment(envList, te);
                printf("Loaded test environment: %s\n", te->label);
            }
            // Reached end of env block
            insideEnvConfigBlock = false;
            continue;
        }
        buf[strcspn(buf, "\n")] = '\0'; // null-term the read line

        // If encountering an env block...
        if(buf[0] == '[') {
            insideEnvConfigBlock = true;
            int count = 1;
            envNameBuf[0] = buf[count];
            while(buf[count+1] != ']') {
                envNameBuf[count] = buf[count+1];
                count++;
            }

            // Null term the toml env key
            envNameBuf[count] = '\0';

            // Save environment key
            te->label = strdup(envNameBuf);
            // printf("\nLoading test environment from toml: %s\n", te.label);
            continue;
        } 
        // Else encountering a property in an env block
        else { 

            // Reset the buffers
            memset(envPropKeyBuf, 0, sizeof(envPropKeyBuf));
            memset(envPropValueBuf, 0, sizeof(envPropValueBuf));

            // Read the property key
            int count = 0;
            while(buf[count] != ' ' && buf[count] != '=') {
                envPropKeyBuf[count] = buf[count];
                count++;
            }
            envPropKeyBuf[count] = '\0'; // null-term the string
            envPropKey = strdup(envPropKeyBuf);

            // Find the property value offset from the property key
            // by iterating past whitespace or the equals sign
            count++;
            while(buf[count] == ' ' || buf[count] == '=') {
                count++;
            }

            // Read the property value
            int i = 0;
            while(count < strlen(buf)) {
                envPropValueBuf[i] = buf[count];
                i++;
                count++;
            }
            envPropValueBuf[strlen(envPropValueBuf)] = '\0';
            envPropValue = strdup(envPropValueBuf);
            
            // Set environment property
            setEnvironmentProperty(envPropKey, envPropValue, te);     

            // Free stdup'd char*'s
            free(envPropKey); 
            free(envPropValue);      
        }


    }

    return envList;
}

char** parseStartCommands(TestEnvironment* te, int* nCommands) {
    char** commands;
    char** finalCommands;
    char buf[256];


    // Malloc memory for commands
    commands = (char**)malloc(MAX_START_COMMANDS * sizeof(char*));
    if(commands == NULL ) {
        perror("Error mallocing commands\n");
    }

    // Read entire start value 
    int startIndex = 1;
    int bufIndex = 0;
    while(te->start[startIndex] != ']') {
        buf[bufIndex] = te->start[startIndex]; // +1 to get past the first bracket
        startIndex++; bufIndex++;
    }
    buf[bufIndex] = '\0';



    // Parse commands by comma
    char* tok = strtok(buf, ",");

    int nTokens = 0;
    while(tok != NULL) {
        commands[nTokens] = strdup(tok);
        tok = strtok(NULL, ",");
        nTokens++;
        (*nCommands)++;
    }

    // Remove quotes from each commands
    for(int j = 0; j < (*nCommands); j++) {
        commands[j][strlen(commands[j])-1] = '\0'; // null-term both commands
        finalCommands[j] = strdup(&commands[j][1]);
    }

    free(commands);

    return finalCommands;
}

void freeTestEnvironment(TestEnvironment* te) {
    free(te->label);
    free(te->projectRoot);
    free(te->start);
    free(te->stop);
    free(te);
}

int testEnvironmentUnset(TestEnvironment* te, int debugMode) {
    int isUnset;


    if (te == NULL) {
        isUnset = 1;
    } else {
        isUnset = 0;
    }
    
    if(debugMode == 1) {
        if(isUnset == 1) {
            printf("The test environment is UNSET.\n");
        } else {
            printf("The test environment is SET.\n");
        }
    } 

    return isUnset;
}