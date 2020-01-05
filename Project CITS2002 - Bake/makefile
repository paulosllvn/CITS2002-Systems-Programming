CC=cc
CFLAGS= -std=c99 -Wall -pedantic -Werror -g
MAIN = main
DEPS = parseFile.c fileReader.c variables.c target.c

$(MAIN): $(DEPS)
	$(CC) $(CFLAGS) -o $(MAIN) $(DEPS)