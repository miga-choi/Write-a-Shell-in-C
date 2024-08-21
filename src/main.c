#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

/*
 * Function Declarations for builtin shell commands:
 */
int sh_cd(char **args);

int sh_help(char **args);

int sh_exit(char **args0);

/*
 * List of builtin commands, followed by their corresponding functions.
 */
char *builtin_str[] = {
        "cd",
        "help",
        "exit"
};

int (*builtin_func[])(char **) = {
        &sh_cd,
        &sh_help,
        &sh_exit
};

int sh_num_builtins() {
    return sizeof(builtin_str) / sizeof(char *);
}

/*
 * Builtin function implementations.
 */
/**
 * @brief Builtin command: change directory.
 * @param args List of args. args[0] is "cd". args[1] is the directory.
 * @return Always returns 1, to continue executing.
 */
int sh_cd(char **args) {
    if (args[1] == NULL) {
        fprintf(stderr, "sh: expected argument to \"cd\"\n");
    } else {
        if (chdir(args[1]) != 0) {
            perror("sh");
        }
    }
    return 1;
}

/**
 * @brief Builtin command: print help.
 * @param args List of args. Not examined.
 * @return Always returns 1, to continue executing.
 */
int sh_help(char **args) {
    int i;
    printf("SH\n");
    printf("Type program names and arguments, and hit enter.\n");
    printf("The following are built in:\n");

    for (int i = 0; i < sh_num_builtins(); i++) {
        printf("  %s\n", builtin_str[i]);
    }

    printf("Use the man command for information on other programs.\n");
    return 1;
}

/**
 * @brief Builtin command: exit.
 * @param args List of args. Not examined.
 * @return Always returns 0, to terminate execution.
 */
int sh_exit(char **args) {
    return 0;
}

/**
 * @brief Launch a program and wait for it to terminate.
 * @param args Null terminated list of arguments (including program).
 * @return Always return 1, to continue execution.
 */
int sh_launch(char **args) {
    pid_t pid, wpid;
    int status;

    pid = fork();
    if (pid == 0) {
        // Child process
        if (execvp(args[0], args) == -1) {
            perror("sh");
        }
        exit(EXIT_FAILURE);
    } else if (pid < 0) {
        // Error forking
        perror("sh");
    } else {
        // Parent process
        do {
            wpid = waitpid(pid, &status, WUNTRACED);
        } while (!WIFEXITED(status) && !WIFSIGNALED(status));
    }

    return 1;
}

/**
 * @brief Execute shell built-in or launch program.
 * @param args Null terminated list of arguments.
 * @return 1 if the shell should continue running, 0 if it should terminate
 */
int sh_execute(char **args) {
    int i;

    if (args[0] == NULL) {
        // An empty command was entered.
        return 1;
    }

    for (i = 0; i < sh_num_builtins(); i++) {
        if (strcmp(args[0], builtin_str[i]) == 0) {
            return (*builtin_func[i])(args);
        }
    }

    return sh_launch(args);
}

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


#define SH_READ_LINE_BUFFER_SIZE 1024

/**
 * @brief Read a line of input from stdin.
 * @return The line from stdin.
 */
char *sh_read_line() {
#ifdef SH_USE_STD_GETLINE
    char *line = NULL;
    ssize_t buffer_size = 0; // have getline allocate a buffer for us
    if (getline(&line, &buffer_size, stdin) == -1) {
        if (feof(stdin)) {
            exit(EXIT_SUCCESS); // We received an EOF
        } else {
            perror("sh: getline\n");
            exit(EXIT_FAILURE);
        }
    }
    return line;
#else
#define SH_READ_LINE_BUFFER_SIZE 1024
    int buffer_size = SH_READ_LINE_BUFFER_SIZE;
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

        if (c == EOF) {
            exit(EXIT_SUCCESS);
        } else if (c == '\n') {
            buffer[position] = '\0';
            return buffer;
        } else {
            buffer[position] = c;
        }
        position++;

        // If we have exceeded the buffer, reallocate.
        if (position >= buffer_size) {
            buffer_size += SH_READ_LINE_BUFFER_SIZE;
            buffer = realloc(buffer, buffer_size);
            if (!buffer) {
                fprintf(stderr, "sh: allocation error\n");
                exit(EXIT_FAILURE);
            }
        }
    }
#endif
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