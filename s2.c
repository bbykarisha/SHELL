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
    return 0;
}

int
redirection_command(Command *command)
{
    return 0;
}

int
pipeline_command(Command *command)
{
    return 0;
}

int
sequence_command(Command *command)
{
    return 0;
}

int
main(void)
{
    return 0;
}