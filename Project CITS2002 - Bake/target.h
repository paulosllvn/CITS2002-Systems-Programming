#include <stdlib.h>
#include "uthash.h"

struct command{
    char *command;
    int mustPrint, mustSuccess, mustExecute;
};

struct target{
    char name[255]; 
    size_t depLength;
    char **dependecies;
    size_t comLength;
    struct command *commands;
    UT_hash_handle hh;
};

struct target *addTarget(char *str, size_t len);
void addCommand(struct target *target, char *str, size_t len, int mustPrint, int mustSuccess, int mustExecute);
void executeTarget(char *name);
void printTargets();