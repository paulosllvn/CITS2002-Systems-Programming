#include "fileReader.h"
#include "target.h"
#include <sys/types.h> 
#include <unistd.h> 
#include <sys/wait.h>

struct target *def = NULL;
struct target *targets = NULL;

struct target *addTarget(char *str, size_t len){
    int i, j, k;
    struct target *curr = (struct target *) malloc (sizeof(struct target));

    curr->depLength = 5;
    curr->dependecies = (char **) malloc (sizeof(char *) * curr->depLength);

    // Delete space and tab.
    for (i = 0; i < len && (str[i] == ' ' || str[i] == '\t'); ++i) ;
    j = i;

    // Find target name.
    for (; i < len && str[i] != ' ' && str[i] != '\t' && str[i] != ':'; ++i) ;
    strncpy(curr->name, str + j, i - j);

    // Skip spaces.
    for (; i < len && (str[i] == ' ' || str[i] == '\t' || str[i] == ':'); ++i) ;

    // Find dependencies
    for (j = i, k = 0; i < len + 1; ++i){
        if (i == len || str[i] == ' ' || str[i] == '\n'){
            if ( i - j > 0 ){
                curr->dependecies[k] = (char *)malloc(i - j);
                strncpy(curr->dependecies[k], str + j, i - j);

                // Reallocate dependecies array if full.
                if (++k == curr->depLength){
                    curr->depLength += 5;
                    curr->dependecies = (char **)realloc(curr->dependecies, sizeof(char *) * curr->depLength);
                }
            }

            j = i + 1;
        }
    }

    curr->depLength = k;
    curr->dependecies = (char **)realloc(curr->dependecies, sizeof(char *) * curr->depLength);

    // Add it to the table of dependecies.
    HASH_ADD_STR(targets, name, curr);

    // Set default target.
    if (def == NULL)
        def = curr;

    return curr;
}

void addCommand(struct target *target, char *str, size_t len, int mustPrint, int mustSuccess, int mustExecute){
    struct command *newCommand;
    int i = 0;

    // Add command.
    target->commands = (struct command *)realloc(target->commands, sizeof(struct command) * ++target->comLength);
    newCommand = target->commands + target->comLength - 1;

    // Check for special symbol.
    newCommand->mustPrint = mustPrint == 2 ? !(str[0] == '@' || (str[1] == '-' && str[2] == '@')) : mustPrint;
    newCommand->mustSuccess = mustSuccess == 2 ? !(str[0] == '-' || (str[1] == '@' && str[2] == '-')) : mustSuccess;
    newCommand->mustExecute = mustExecute;

    // Save command string.
    newCommand->command = (char *) malloc (len);
    while (str[i] == '\t' || str[i] == ' ') ++i;
    strcpy(newCommand->command, str + i);
}

void getDateFromCurl(){

}

void executeCommands(struct target *curr){
    int i = 0;
    // char path[255] = "", arg[255] = "";
    struct command com;
    pid_t pid;

    for (i = 0; i < curr->comLength; ++i){
        com = curr->commands[i];
        
        if (com.mustPrint)
            printf("%s\n", com.command);
        
        if(0 && com.mustExecute){
            pid=fork();
            
            if (pid==0) {           // Child executes command.
                execl(getenv("SHELL") == NULL ? "/bin/bash" : getenv("SHELL"), com.command);
                exit(127); /* only if execv fails */
            }
            else {                  // Parent waits for child.
                if( waitpid(pid,0,0) == -1 && com.mustSuccess){
                    printf("command: %s failed!", com.command);
                    exit(1);
                }
            }
        }
        
    }
}

int executeIfNeeded(struct target *curr){
    int needExecution = 0, existDep, i;
    char *dep;
    __time_t lastMod, nextMod;
    struct target *next = NULL;

    // Check if file exists.
    needExecution = !existFile(curr->name, &lastMod);

    // Check dependencies.
    for ( i = 0; i < curr->depLength; ++i){
        dep = curr->dependecies[i];
        if (!strncmp(dep, "file://", 7) || !strncmp(dep, "http://", 7) || !strncmp(dep, "https://", 8) ){

        }
        else{
            // Check if the dependency has a modification time more recent.
            existDep = existFile(dep, &nextMod);
            if (!needExecution && existDep)
                needExecution = difftime(nextMod, lastMod) > 0;

            // If the dependency is also a target, execute it.
            HASH_FIND_STR(targets, dep, next);
            if (next != NULL)
                needExecution |= executeIfNeeded(next);

            // If the dependency is not a file nor a dependency, exit with error.
            if (!existDep && next == NULL){
                printf("%s is not a file nor a target\n", dep);
                exit(1);
            }
        }
    }

    // printf("%f", difftime(lastMod, 1540393567));
    if (needExecution)
        executeCommands(curr);

    return needExecution;
}

void executeTarget(char *name){
    struct target *tmp = NULL;

    // Check if the passed name exists as targer, otherwise use default one.
    if(name != NULL){
        HASH_FIND_STR(targets, name, tmp);
        if (tmp != NULL)
            def = tmp;
        else{
            printf("The target %s does not exist!", name);
            exit(1);
        }
    }
    
    executeIfNeeded(def);
}

void printTargets(){
    size_t i = 0;
    struct target *s;

    for(s = targets; s != NULL; s = s->hh.next) {
        printf("Target: %s\nDependencies: ", s->name);
        
        for (i = 0; i < s->depLength; ++i)
            printf("%s ", s->dependecies[i]);

        printf("\nCommands: ");

        for (i = 0; i < s->comLength; ++i)
            printf("%s\n", s->commands[i].command);  
        
        printf("\n");
    }
}