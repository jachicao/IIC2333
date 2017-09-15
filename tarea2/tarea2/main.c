#include "main.h"

int exit_(int argc, char** argv)
{
    return EXIT_FAILURE;
}

int setPrompt(int argc, char** argv)
{
    if (argc < 2)
    {
        printf("setPrompt: expected two arguments\n");
        return EXIT_FAILURE;
    }
    else
    {
        
    }
    return EXIT_SUCCESS;
}

int setPath(int argc, char** argv)
{
    if (argc < 2)
    {
        printf("setPath: expected two arguments\n");
        return EXIT_FAILURE;
    }
    else
    {
        if (chdir(argv[1]) != 0)
        {
            perror("setPath");
        }
    }
    return EXIT_SUCCESS;
}

int launch(int argc, char** argv)
{
    pid_t pid = fork();
    if (pid == 0) {
        // Child process
        if (execvp(argv[0], argv) == -1)
        {
            perror("lsh");
        }
        exit(EXIT_FAILURE);
    }
    else
    {
        int status;
        do
        {
            waitpid(pid, &status, WUNTRACED);
        } while (!WIFEXITED(status) && !WIFSIGNALED(status));
    }
    
    return EXIT_SUCCESS;
}

int execute(int argc, char** argv)
{
    if (argc == 0)
    {
        return EXIT_SUCCESS;
    }
    
    for (int i = 0; i < builtin_funcs_size; i++)
    {
        if (strcmp(argv[0], builtin_funcs_str[i]) == 0)
        {
            return (*builtin_funcs[i])(argc, argv);
        }
    }
    
    return launch(argc, argv);
}

char** split(char* command, int* size)
{
    char* separator = " ";
    int char_size = sizeof(char*);
    char** args = (char**) malloc(char_size);
    if (args == NULL)
    {
        printf("malloc error\n");
        exit(EXIT_FAILURE);
    }
    
    char* token = strtok(command, separator);
    int i = 0;
    while (token != NULL) {
        args[i] = token;
        i++;
        
        args = (char**) realloc(args, char_size * (i + 1));
        if (args == NULL)
        {
            printf("realloc error\n");
            exit(EXIT_FAILURE);
        }
        
        token = strtok(NULL, separator);
    }
    *size = i;
    args[i] = NULL;
    return args;
}

char* read_next()
{
    int char_size = sizeof(char);
    char* buffer = (char*) malloc(char_size);
    if (buffer == NULL)
    {
        printf("malloc error\n");
        exit(EXIT_FAILURE);
    }
    
    int i = 0;
    while (true)
    {
        char c = getchar();
        if (c == EOF || c == '\n')
        {
            break;
        }
        else
        {
            buffer[i] = c;
        }
        i++;
        buffer = (char*) realloc(buffer, char_size * (i + 1));
        if (buffer == NULL)
        {
            printf("realloc error\n");
            exit(EXIT_FAILURE);
        }
    }
    buffer[i] = '\0';
    return buffer;
}

void free_vars()
{
    if (next_command != NULL)
    {
        free(next_command);
    }
    if (next_argv != NULL)
    {
        free(next_argv);
    }
    next_command = NULL;
    next_argc = 0;
    next_argv = NULL;
}

void on_sigint(int s)
{
    //free_vars();
    //exit(EXIT_SUCCESS);
}

int main(int argc, char **argv)
{
    signal(SIGINT, on_sigint);
    
    int exit_code = EXIT_SUCCESS;
    while (exit_code == EXIT_SUCCESS)
    {
        printf("%s", prompt);
        next_command = read_next();
        next_argv = split(next_command, &next_argc);
        exit_code = execute(next_argc, next_argv);
        free_vars();
    }
    
    return EXIT_SUCCESS;
}
