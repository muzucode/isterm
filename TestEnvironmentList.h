#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>


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
    list->environments = (TestEnvironment*)malloc(list->capacity * sizeof(TestEnvironment));
}

// Add a TestEnvironment to the list
void addTestEnvironment(TestEnvironmentList *list, TestEnvironment *env) {
    // Resize the array if needed
    if (list->size >= list->capacity) {
        list->capacity *= 2;
        list->environments = (TestEnvironment*)realloc(list->environments, list->capacity * sizeof(TestEnvironment));
    }
    list->environments[list->size++] = *env;
}


TestEnvironment* findTestEnvironmentByLabel(TestEnvironmentList *list, const char *label) {
    for (size_t i = 0; i < list->size; i++) {
        if (strcmp(list->environments[i].label, label) == 0) {
            return &list->environments[i];
        }
    }
    return NULL; // Not found
}