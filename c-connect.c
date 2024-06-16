#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

typedef struct {
    char* ipv4;
    char* name;
} User;


void printWelcomeMessage(User* user) {
    printf("Greetings to the new host on the network: %s\n", user->name);
}

char* promptUserName() {
    char buf[64];
    char* name;

    printf("Enter username: ");
    fgets(buf, sizeof(buf), stdin);
    buf[strcspn(buf, "\n")] = '\0';

    name = (char*)malloc(strlen(buf) + 1);
    strcpy(name, buf);

    return name;
}


int main() {

    User* user = (User*)malloc(sizeof(User*));
    user->name = promptUserName();

    printWelcomeMessage(user);


    free(user->name);
    free(user);

    return 0;
}