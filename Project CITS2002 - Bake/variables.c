#include "variables.h"
#include "uthash.h"
#include <string.h>
#include <stdio.h>
#include <unistd.h>

struct variable{
    char id[255];
    char content[255];
    UT_hash_handle hh;
};

// Hash table for variables.
struct variable *variables = NULL;

void saveVariable(char *line, size_t len){
    struct variable *newVar = (struct variable *)malloc (sizeof(struct variable));
    size_t i, j = 0;

    // Variable name.
    for (i = 0; i < len && line[i] != ' ' && line[i] != '='; ++i)
        newVar->id[j++] = line[i];

    // Go to variable declaration.
    while (line[i] == ' ' || line[i] == '=')
        ++i;

    // Save variable declaration.
    j = 0;
    for (; i < len && line[i] != '\n'; ++i)
        newVar->content[j++] = line[i];

    strcpy(newVar->content, substituteVariable(newVar->content, strlen(newVar->content), &len));
    // Add variables to hash table.
    HASH_ADD_STR(variables, id, newVar);
}


char *substituteVariable(char *str, size_t size, size_t *newLen){
    int i = 0, j = 0, k = 0, l = 0;
    size_t currSize = size;
    char name[255] = "", *newStr = (char *) malloc (size), content[255] = "";
    struct variable *tmp = NULL;

    for (; i < size; ++i){
        // Check if variable expansion is needed.
        if (str[i] == '$' && str[i + 1] == '('){
            for (j = i + 2, k = 0; str[j] != ')' && j < size; ++j, ++k)
                name[k] = str[j];

            if (j < size){
                // Variable has been found
                if (!strcmp(name, "PID"))
                    snprintf(content, 255, "%i", getpid());
                else if(!strcmp(name, "PPID"))
                    snprintf(content, 255, "%i", getppid());
                else if(!strcmp(name, "PWD"))
                    getcwd(content, sizeof(content));
                else if(!strcmp(name, "RAND"))
                    snprintf(content, 255, "%i", rand());
                else{
                    HASH_FIND_STR(variables, name, tmp);
                    if(tmp != NULL)                         // Variable stored in hash table.
                        strcpy(content, tmp->content);
                    else if(getenv(name) != NULL)           // Varible in global environment.
                        strcpy(content, getenv(name));      
                    else{                                   // ERROR.
                        printf ("Variable %s does not exist.\n", name);
                        exit(1);
                    }
                }
                
                // Realloc new string if too short
                if (l + strlen(content) >= currSize){
                    currSize += strlen(content) + 10;
                    newStr = (char*)realloc(newStr, currSize);
                }

                // Copy content.
                for (k = 0; k < strlen(content); ++k)
                    newStr[l++] = content[k];

                // Update position.
                i = j;
            }

            memset(name, 0, 255);
            memset(content, 0, 255);
        }
        else{
            newStr[l++] = str[i];

            // Realloc string if too short.
            if (l >= currSize){
                currSize += 20;
                newStr = (char *) realloc (newStr, currSize);
            }
        }
    }

    *newLen = l;
    newStr = (char*)realloc(newStr, l);
    return newStr;
}