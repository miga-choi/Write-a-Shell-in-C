#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SH_TOKEN_BUFFER_SIZE 64
#define SH_TOKEN_DELIMITER " \t\r\n\a"

/**
 * @brief Split a line into tokens (very naively).
 * @param line The line.
 * @return Null-terminated array of tokens.
 */
char **sh_split_line(char *line) {
    int buffer_size = SH_TOKEN_BUFFER_SIZE, position = 0;
    char **tokens = malloc(buffer_size * sizeof(char *));
    char *token;

    if (!tokens) {
        fprintf(stderr, "sh: allocation error\n");
        exit(EXIT_FAILURE);
    }

    token = strtok(line, SH_TOKEN_DELIMITER);

    while (token != NULL) {
        tokens[position] = token;
        position++;

        if (position >= buffer_size) {
            buffer_size += SH_TOKEN_BUFFER_SIZE;
            tokens = realloc(tokens, buffer_size * sizeof(char *));
            if (!tokens) {
                fprintf(stderr, "sh: allocation error\n");
                exit(EXIT_FAILURE);
            }
        }

        token = strtok(NULL, SH_TOKEN_DELIMITER);
    }

    tokens[position] = NULL;
    return tokens;
}

#define SH_REAR_LINE_BUFFER_SIZE 1024

/**
 * @brief Read a line of input from stdin.
 * @return The line from stdin.
 */
char *sh_read_line(void) {
    int buffer_size = SH_REAR_LINE_BUFFER_SIZE;
    int position = 0;
    char *buffer = malloc(sizeof(char) * buffer_size);
    int c;

    if (!buffer) {
        fprintf(stderr, "sh: allocation error\n");
        exit(EXIT_FAILURE);
    }

    while (1) {
        // Read a character
        c = getchar();

        // If we hit EOF(End of File), replace it with a null character and return.
        if (c == EOF || c == '\n') {
            buffer[position] = '\0';
            return buffer;
        } else {
            buffer[position] = c;
        }
        position++;

        // If we have exceeded the buffer, reallocate.
        if (position >= buffer_size) {
            buffer_size += SH_REAR_LINE_BUFFER_SIZE;
            buffer = realloc(buffer, buffer_size);
            if (!buffer) {
                fprintf(stderr, "sh: allocation error\n");
                exit(EXIT_FAILURE);
            }
        }
    }
}

//char *sh_read_line(void) {
//    char *line = NULL;
//    ssize_t buffer_size = 0; // have getline allocate a buffer for us
//
//    if (getline(&line, &buffer_size, stdin) == -1) {
//        if (feof(stdin)) {
//            exit(EXIT_SUCCESS); // We recieved an EOF
//        } else {
//            perror("readline");
//            exit(EXIT_FAILURE);
//        }
//    }
//
//    return line;
//}

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

        // Read the command from standard input.
        line = sh_read_line();

        // Separate the command string into a program and arguments.
        args = sh_split_line(line);

        // Run the parsed command.
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