#include "fileReader.h"
#include "parseFile.h"
#include "variables.h"
#include "target.h"
#include <unistd.h>

int executeCommand = 1, printCommand = 2, careCommand = 2, shouldExecute = 1;

int main(int argc, char **argv){
    char *filename = "bakefile";
    FILE *file;
    int i;

    // Read arguments
    for (i = 1; i < argc; ++i){
        if (!strcmp(argv[i], "-C")){        // Change dir.
            if (chdir(argv[i + 1]) < 0){
                printf("Directory %s does not exist \n", argv[i+1]);
                exit(1);
            }
            ++i;
        }
        else if (!strcmp(argv[i], "-f")){   // Change file.
            if(existFile(argv[i + 1], NULL))
                strcpy(filename, argv[i]);
            else{
                printf("File %s does not exist \n", argv[i+1]);
                exit(1);
            }
            ++i;
        }
        else if (!strcmp(argv[i], "-i")){   // Do not wait command to be successful.
            careCommand = 0;
        }
        else if (!strcmp(argv[i], "-n")){   // Just print commands.
            printCommand = 1; executeCommand = 0;
        }
        else if (!strcmp(argv[i], "-p")){   // Print info.
            shouldExecute = 0;
        }
        else if (!strcmp(argv[i], "-s")){   // Do not print command.
            printCommand = 0;
        }
        else{
            printf("\"%s\" Argument not valid.\n", argv[i]);
            exit(1);
        }

    }

    if ((file = fopen(filename,"r")) == NULL){
        printf("Error! opening file");

        // Program exits if the file pointer returns NULL.
        exit(1);
    }

    execute(file);
}

void execute(FILE *file){
    char *line = NULL, type, lastType = ERROR, *lineExpanded = NULL;
    ssize_t len;
    size_t len2 = 0;
    struct target *lastTarget;

    setFile(file);

    while ((type = getNextRowType(&line, &len)) != ERROR) {
        // Switch based on the type of the current line.
        switch(type){
            case COMMENT:
                break;
            case VARIABLE:
                saveVariable(line, len);
                break;
            case TARGET:
                lineExpanded = substituteVariable(line, len, &len2);
                lastTarget = addTarget(lineExpanded, len2);
                break;
            case ACTION:
                if (lastType != ACTION && lastType != TARGET){
                    printf("Error parsing the file");
                    exit(1);
                }
                lineExpanded = substituteVariable(line, len, &len2);
                addCommand(lastTarget, lineExpanded, len2, printCommand, careCommand, executeCommand);
                break;
        }
        lastType = type;
    }

    // Executes command or print them.
    if(shouldExecute)
        executeTarget(NULL);
    else
        printTargets();
}
