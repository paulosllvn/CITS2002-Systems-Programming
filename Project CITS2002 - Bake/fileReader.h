#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define ERROR -1
#define COMMENT 0
#define VARIABLE 1
#define TARGET 2
#define ACTION 3

void setFile(FILE *fp);
char getNextRowType(char **row, ssize_t *len);
int existFile(char *name, __time_t *time);