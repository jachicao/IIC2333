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
        int char_size = sizeof(char);
        char* new_prompt = (char*) malloc(char_size);
        new_prompt[0] = '\0';
        for (int i = 1; i < argc; i++)
        {
            char* str = argv[i];
            size_t str1_len = strlen(new_prompt);
            size_t str2_len = strlen(str);
            for (int j = 0; j < str2_len; j++)
            {
                if (str[j] == '*')
                {
                    str[j] = next_exit_code + '0';
                }
            }
            size_t new_size = str1_len + str2_len + 2;
            new_prompt = (char*) realloc(new_prompt, new_size);
            memcpy(new_prompt + str1_len, str, str2_len + 1);
            memcpy(new_prompt + str1_len + str2_len, " ", 1);
        }
        if (prompt != NULL)
        {
            free(prompt);
        }
        prompt = new_prompt;
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
    char* last_arg = argv[argc - 1];
    if (last_arg[0] == '&')
    {
        if (strlen(last_arg) > 1)
        {
            
        }
        else
        {
            
        }
        return EXIT_SUCCESS;
    }
    else
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

void on_exit(int s)
{
    free_vars();
    if (prompt != NULL)
    {
        free(prompt);
    }
    exit(s);
}

int main(int argc, char **argv)
{
    prompt = strdup(first_prompt);
    signal(SIGINT, on_exit);
    while (next_exit_code == EXIT_SUCCESS)
    {
        printf("%s", prompt);
        next_command = read_next();
        next_argv = split(next_command, &next_argc);
        next_exit_code = execute(next_argc, next_argv);
        free_vars();
    }
    
    on_exit(EXIT_SUCCESS);
    
    return EXIT_SUCCESS;
}
