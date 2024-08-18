#include <stdlib.h>
#include <stdio.h>

/**
 * @brief Read a line of input from stdin.
 * @return The line from stdin.
 */
char *sh_read_line() {
}

/**
 * @brief Split a line into tokens (very naively).
 * @param line The line.
 * @return Null-terminated array of tokens.
 */
char **sh_split_line(char *line) {
}


/**
 * @brief Execute shell built-in or launch program.
 * @param args Null terminated list of arguments.
 * @return 1 if the shell should continue running, 0 if it should terminate
 */
int sh_execute(char **args) {
}

/**
 * @brief Loop getting input and executing it.
 */
void sh_loop() {
    char *line;
    char **args;
    int status;

    do {
        printf("> ");
        line = sh_read_line();
        args = sh_split_line(line);
        status = sh_execute(args);

        free(line);
        free(args);
    } while (status);
}

/**
 * @brief Main entry point.
 * @param argc Argument count.
 * @param argv Argument vector.
 * @return status code.
 */
int main(int argc, char **argv) {
    // Load config files, if any.

    // Run command loop.
    sh_loop();

    // Perform any shutdown/cleanup.
    return EXIT_SUCCESS;
}