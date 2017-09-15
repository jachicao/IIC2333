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

int launch_in_background(int argc, char** argv)
{
    pid_t pid = fork();
    if (pid == 0)
    {
        if (execvp(argv[0], argv) == -1)
        {
            perror("msh");
        }
        exit(EXIT_FAILURE);
    }
    return EXIT_SUCCESS;
}

int launch_not_in_background(int argc, char** argv)
{
    pid_t pid = fork();
    if (pid == 0)
    {
        if (execvp(argv[0], argv) == -1)
        {
            perror("msh");
        }
        exit(EXIT_FAILURE);
    }
    else
    {
        child_pid = pid;
        int status;
        waitpid(pid, &status, 0);
    }
    
    return EXIT_SUCCESS;

}

int launch(int argc, char** argv)
{
    char* last_arg = argv[argc - 1];
    if (last_arg[0] == '&')
    {
        argv[argc - 1] = NULL;
        argc--;
        size_t str_len = strlen(last_arg);
        if (str_len > 1)
        {
            char* int_str = (char*) malloc(str_len);
            for (int i = 1; i < str_len; i++)
            {
                int_str[i - 1] = last_arg[i];
            }
            
            int N = atoi(int_str);
            
            for (int i = 0; i < N; i++)
            {
                launch_in_background(argc, argv);
            }
            
            free(int_str);
        }
        else
        {
            launch_in_background(argc, argv);
        }
        return EXIT_SUCCESS;
    }
    else
    {
        return launch_not_in_background(argc, argv);
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
    char* separator = " \t\r\n\a";
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
    child_pid = -1;
}

void on_program_exit(int s)
{
    if (prompt != NULL)
    {
        free(prompt);
    }
    free_vars();
    exit(s);
}

void on_program_kill(int s)
{
    if (child_pid > 0)
    {
        kill(child_pid, SIGTERM);
        free_vars();
    }
    else
    {
        on_program_exit(s);
    }
}

int main(int argc, char **argv)
{
    signal(SIGINT, on_program_kill);
    /*
    struct sigaction sig_handler;
    sig_handler.sa_handler = on_kill;
    sigemptyset(&sig_handler.sa_mask);
    sig_handler.sa_flags = 0;
    sigaction(SIGINT, &sig_handler, NULL);
    */
    
    prompt = strdup(first_prompt);
    while (next_exit_code == EXIT_SUCCESS)
    {
        printf("%s", prompt);
        next_command = read_next();
        next_argv = split(next_command, &next_argc);
        next_exit_code = execute(next_argc, next_argv);
        free_vars();
    }
    
    on_program_exit(EXIT_SUCCESS);
    
    return EXIT_SUCCESS;
}
