#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

typedef enum
{
    KIND_SIMPLE = 0,
    KIND_REDIRECT = 1,
    KIND_PIPELINE = 2,
    KIND_SEQ = 3
} Kind;

typedef enum
{
    RD_INPUT = 10,
    RD_OUTPUT = 11,
    RD_APPEND = 12
} Redirection;

typedef enum
{ 
    OP_SEQ = 20,
    OP_DISJUNCT = 21,
    OP_CONJUNCT = 22, 
    OP_BACKGROUND = 23
} Operation;

typedef struct Command 
{
    int kind;
    union
    {
        // KIND_SIMPLE
        struct 
        {
            int argc;
            char **argv;
        }; 
        // KIND_REDIRECT
        struct
        {
            int rd_mode;
            char *rd_path;
            struct Command *rd_command;
        };  
        // KIND_PIPELINE
        struct
        {
            int pipeline_size;
            struct Command *pipeline_commands;
        };
        // KIND_SEQ
        struct
        {
            int seq_size;
            struct Command *seq_commands;
            int *seq_operations;
        };     
    };  
} Command;

int
simple_command(Command *);

int
redirection_command(Command *);
 
int
sequence_command(Command *);

int
pipeline_command(Command *);

int
run_command(Command *command)
{
    int exit_status;
    
    switch (command->kind)
    {
        case KIND_SIMPLE:
            exit_status = simple_command(command);
            break;
        case KIND_REDIRECT:
            exit_status = redirection_command(command);
            break;
        case KIND_PIPELINE:
            exit_status = pipeline_command(command);
            break;
        default:
            exit_status = sequence_command(command);
            break;
    }

    return exit_status;
}

int
simple_command(Command *command)
{
    if (fork() == 0) {
        execvp(command->argv[0], command->argv);
        exit(1);
    }

    int status;
    wait(&status);

    if (WIFEXITED(status)) {
        return WEXITSTATUS(status);
    }

    return 1;
}

int
redirection_command(Command *command)
{   
    int fd = 0;
    int exit_status = 1;

    if (fork() == 0) {
        switch (command->rd_mode) 
        {
            case RD_INPUT:
                fd = open(command->rd_path, O_RDONLY);
                if (fd == -1) {
                    exit(1);
                }
                dup2(fd, STDIN_FILENO);
                close(fd);
                break;
            case RD_OUTPUT:
                fd = open(command->rd_path, O_WRONLY | O_CREAT | O_TRUNC, 0666);
                if (fd == -1) {
                    exit(1);
                }
                dup2(fd, STDOUT_FILENO);
                close(fd);
                break;
            default: //RD_APPEND
                fd = open(command->rd_path, O_WRONLY | O_CREAT | O_APPEND, 0666);
                if (fd == -1) {
                    exit(1);
                }
                dup2(fd, STDOUT_FILENO);
                close(fd);
                break; 
        }
        exit_status = run_command(command->rd_command); 
        exit(exit_status);
    }

    int status;
    wait(&status);

    if (WIFEXITED(status)) {
        return WEXITSTATUS(status);
    }

    return 1;
}

int
sequence_command(Command *command)
{
    int index = 0;
    int background = 0;
    int exit_status = 1;

    // определяем нужно ли ждать завершения процесса или нет
    if (command->seq_operations[command->seq_size - 1] == OP_BACKGROUND) {
        background = 1;
    }

    if (fork() == 0) {
        while (index < command->seq_size) {
            exit_status = run_command(&command->seq_commands[index]); 

            int keep_checking = 1;    
            while (keep_checking == 1) {
                keep_checking = 0;
                switch (command->seq_operations[index])
                {
                    case OP_CONJUNCT:
                        // не выполняем следующую команду, если текущая 
                        // неуспешна
                        if (exit_status != 0) {
                            ++index;
                            keep_checking = 1;
                        }
                        break;
                    case OP_DISJUNCT:
                        // не выполняем следующую команду, если текущая 
                        // успешна
                        if (exit_status == 0) {
                            ++index;
                            keep_checking = 1;
                        }
                        break;
                    default:
                        // либо случай OP_SEQ, либо случай OP_BACKGROUND
                        // определяют только ожидание завершения
                        break;
                }

                if (keep_checking == 0) {
                    ++index;
                }
            }
        }
        exit(exit_status);
    }

    if (background == 1) {
        return 0;
    }

    int status;
    wait(&status);

    if (WIFEXITED(status)) {
        return WEXITSTATUS(status);
    }

    return 1;
}

int
pipeline_command(Command *command)
{
    return 0;
}

int
main(void)
{
    while (wait(NULL) > 0) {}

    return 0;
}