#include "fileReader.h"
#include <unistd.h>
#include <sys/stat.h>

FILE *file = NULL;

void setFile(FILE *fp){
    file = fp;
}

int isBlank(char *line, int len){
    int i = 0;

    for (i = 0; i < len; ++i)
        if (line[i] != ' ' && line[i] != '\t' && line[i] != '\n')
            return 0;

    return 1;
}

char getRowType(char *line, int size){
    int i = 0;

    // If the row start with a #, it is a comment.
    if (line[0] == '#')
        return COMMENT;

    // If the row start with a \t, it is an action.
    if (line[0] == '\t' || (line[0] == ' ' && line[1] == ' ' && line[2] == ' ' && line[3] == ' '))
        return ACTION;

    // Check if it is a varible or a target.
    while (i < size && line[i] != '=' && line[i] != ':')
        ++i;
    
    return i == size ? ACTION : (line[i] == '=') ? VARIABLE : TARGET;
}

char getNextRowType(char **row, ssize_t *length){
    char * line = NULL;
    size_t len = 0;
    ssize_t read;

    // Skip blank lines.
    while((read = getline(&line, &len, file)) != -1 && isBlank(line, read)) ;

    if (read != -1){
        *row = (char *)malloc(read);
        strcpy(*row, line);
        *length = read;
        return getRowType(line, read);
    }

    return ERROR;
}

int existFile(char *name, __time_t *time){
    struct stat attrib;

    // Check if file exists.
    if( access( name, F_OK ) != -1 ) {
        if(time != NULL){
            stat(name, &attrib);
            *time = attrib.st_mtime;
        }
        return 1;
    }

    return 0;
}